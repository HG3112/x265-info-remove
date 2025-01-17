USAGE:

A) x265-info-remove.exe inputFile.hevc outputFile.hevc
B) x265-info-remove.exe all inputFile.hevc outputFile.hevc

The script looks for each occurence of the hex string 00 00 01 4E 01 05 in inputFile and removes it, together with following bytes up to next 00 00 01

If the option 'all' is not specified, the first occurrence is kept

The script is useful to remove unwanted info user-defined nal-units (encoding settings) from raw x265 hevc stream

WARNING: use only on raw .hevc stream ( NOT .mkv NOT .mp4 )