# sam2psl
Convert SAM format to PSL format.

### Usage
```
   make  
   ./sam2psl -h  ## print header, control-c to exit  
   cat tmp.sam | ./sam2psl -h  | grep -v ^#
```

### Notes
- This software is written to be portable. All is included in a single file. To compile, ```g++ -O2 sam2psl.cpp```. Bug reports are apprecaited.
- By default, the original SAM alignments are printed with '#' at the beginning. Use ```grep -v ^#``` to get rid of them.
- This software is only tested on bwa and bowtie2 outputs and may not work for other aligners.
- tStart is reported as -1 if a read is not aligned.
- TLEN is calculated differently by bwa and bowtie2. BWA calculates TLEN by mapped start and end positions; bowtie2 includes soft-clipped bases.
- AS, the mapping score, is calculated differently by different aligners.
- MAPS, the mapping score, is calculated as matched length - edit distance.
- NM, the edit distance, may not be accurately reported by aligners.
- MD, which complements CIGAR to reproduce the matched reference, may not be accurately reported by aligners.
- Blocks, in SAM format, all M/I/D/= blocks are considered matched parts. To conform with PSL format as close as possible, all M blocks are treated as different blocks. For example, a CIGAR with 10S30M4D30M5S produces 2 blocks, with qStarts being 10,40,, blockSizes being 30,30,.  

### Status
- qBlocks and tBlocks are not printed yet, but present in the output as ,'s.
- The other fields should be accurate.

### Output
The columns are:

    matches,     ##1.  Number of matching bases that aren't repeats; note, all matches are included. 
    misMatches,  ##2.  Number of bases that don't match.
    repMatches,  ##3.  Number of matching bases that are part of repeats; always 0, see column 1.  
    nCount,      ##4.  Number of 'N' bases.
    qNumInsert,  ##5.  Number of inserts in query.
    qBaseInsert, ##6.  Number of bases inserted into query.
    tNumInsert,  ##7.  Number of inserts in target.
    tBaseInsert, ##8.  Number of bases inserted into target; commonly refered as deletion.
    strand,      ##9.  '+' or '-'; '*' if cannot be determined.
    qName,       ##10. Query sequence name.
    qSize,       ##11. Query sequence size.
    qStart,      ##12. Alignment start position in query.
    qEnd,        ##13. Alignment end position in query.
    tName,       ##14. Target sequence name.
    tSize,       ##15. Target sequence size.
    tStart,      ##16. Alignment start position in query.
    tEnd,        ##17. Alignment end position in query.
    blockCount,  ##18. Number of blocks in the alignment.
    blockSizes,  ##19. Comma-separated list of sizes of each block.
    qStarts,     ##20. Comma-separated list of start position of each block in query.
    tStarts,     ##21. Comma-separated list of start position of each block in target.

    qBlocks,     ##22. Comma-separated list of sequence blocks in query. 
    tBlocks,     ##23. Comma-separated list of sequence blocks in target on aligned strand reading from 5' to 3'. 

    RI,          ##24. Read index, [1/2].
    RNEXT,       ##25. Mate target, [=/chr].
    PNEXT,       ##26. Mate position, 0-based.
    TLEN,        ##27. Template length, reported by aligner. 
    MAPQ,        ##28. Mapping quality.
    AS,          ##29. Mapping score, reported by the aligner.
    MAPS,        ##30. Mapping score, matched length - edit distance, similar to bwa.
    FPAIRED,     ##31. Paired end or single end, [P/S].
    FPROPER_PAIR,##32. Pairs properly paired or unpaired, [P/U].
    FSECONDARY,  ##33. Primary or secondary mapping, [P/S].    
    FQC,         ##34. Quality control pass or fail, [P/F].
    FDUP,        ##35. Read primary or duplicated, [P/D].

