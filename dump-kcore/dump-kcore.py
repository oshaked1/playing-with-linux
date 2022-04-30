from typing import List, Tuple

import sys
from dataclasses import dataclass


# files needed
IOMEM = '/proc/iomem'
KCORE = '/proc/kcore'

# offsets for header fields
EI_CLASS = 4
EI_DATA = 5
E_PHOFF = 32
E_PHENTSIZE = 54
E_PHNUM = 56

# 64-bit ELF
ELFCLASS64 = 2

# little-endian
ELFDATA2LSB = 1

# offsets for program header fields
P_TYPE = 0
P_OFFSET = 8
P_PADDR = 24
P_MEMSZ = 40

# LOAD segment
PT_LOAD = 1

# LiME constants
LIME_MAGIC = 0x4C694D45
LIME_VERSION = 1


@dataclass
class Segment:
    offset: int
    paddr: int
    size: int


def parse_iomem_regions() -> List[Tuple[int, int]]:
    with open(IOMEM, 'r') as f:
        lines = f.read().splitlines()
    
    ranges = []

    for line in lines:
        parts = line.split(':')

        # check if it is a system RAM region
        if parts[1].strip() == 'System RAM':
            start = int(parts[0].split('-')[0].strip(), 16)
            end = int(parts[0].split('-')[1].strip(), 16)
            ranges.append((start, end))
    
    return ranges


def parse_kcore_segments() -> List[Segment]:
    with open(KCORE, 'rb') as f:
        # read ELF class
        f.seek(EI_CLASS)
        elf_class = int.from_bytes(f.read(1), byteorder='little')

        # make sure it's a 64-bit system
        if elf_class != ELFCLASS64:
            print("Only 64-bit systems are supported")
            exit()

        # read ELF data format
        f.seek(EI_DATA)
        elf_data = int.from_bytes(f.read(1), byteorder='little')
        byteorder = 'little' if elf_data == ELFDATA2LSB else 'big'
        
        # read program header table offset
        f.seek(E_PHOFF)
        phoff = int.from_bytes(f.read(8), byteorder=byteorder)

        # read program header entry size
        f.seek(E_PHENTSIZE)
        phentsize = int.from_bytes(f.read(2), byteorder=byteorder)

        # read program header num
        f.seek(E_PHNUM)
        phnum = int.from_bytes(f.read(2), byteorder=byteorder)

        # read program headers
        segments = []
        for i in range(phnum):
            f.seek(phoff + i*phentsize)
            phent_raw = f.read(phentsize)

            # get type
            p_type = int.from_bytes(phent_raw[P_TYPE: P_TYPE+4], byteorder=byteorder)

            # make sure it is a LOAD segment
            if not p_type == PT_LOAD:
                continue
            
            # get offset, paddr and size
            offset = int.from_bytes(phent_raw[P_OFFSET: P_OFFSET+8], byteorder=byteorder)
            paddr = int.from_bytes(phent_raw[P_PADDR: P_PADDR+8], byteorder=byteorder)
            size = int.from_bytes(phent_raw[P_MEMSZ: P_MEMSZ+8], byteorder=byteorder)

            # add segment to list
            segments.append(Segment(offset=offset, paddr=paddr, size=size))
    
    return segments


def generate_lime_header(start: int, end: int) -> bytes:
    return (LIME_MAGIC.to_bytes(4, byteorder='little') + LIME_VERSION.to_bytes(4, byteorder='little') +
            start.to_bytes(8, byteorder='little') + end.to_bytes(8, byteorder='little') + b'\x00'*8)


def dump_kcore(outfile: str):
    # get virtual address ranges of identity-mapped physical memory
    phys_mem_ranges = parse_iomem_regions()

    # get LOAD segments of kcore file
    segments = parse_kcore_segments()
    
    # dump physical memory ranges according to their offsets in the kcore file
    with open(KCORE, 'rb') as ifh:
        with open(outfile, 'wb') as ofh:
            for mem_range in phys_mem_ranges:
                for segment in segments:
                    # memory region is contained in segment
                    if segment.paddr <= mem_range[0] <= segment.paddr + segment.size:
                        # seek to offset of this physical memory region in the kcore file
                        ifh.seek(segment.offset + mem_range[0] - segment.paddr)

                        # calculate end address
                        end = min(mem_range[1], segment.paddr + segment.size)

                        # discard partial page at end of range
                        if end % 4096 != 0:
                            end -= end % 4096

                        # write a LiME header of this region to the output file
                        ofh.write(generate_lime_header(mem_range[0], end - 1))

                        # write the memory range from the kcore file to the output file
                        ofh.write(ifh.read(end - mem_range[0]))
                        
                        # continue to next physical memory range
                        break


if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("An output file is required")
        exit()

    dump_kcore(sys.argv[1])
