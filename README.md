# UTF-16-Handler
C program transform a UTF-16 txt file to another OS. <br />
The program supports 3 options (Multiple OS support (Mac, Unix, Windows)):
- Copy an existing UTF-16 file
- Transform from\to Big-Endian to\from Little-Endian

## How to use

After using `make` you can use the following command-line arguments:
  
`source-file-name new-file-name [source-file-os-flag] [new-file-os-flag] [byte-order-flag]`

### Parameters

**Notice that the default is .txt files, but you can use other files (but you have to be consistent).**

Name | Meaning 
-----|-------
`source-file-name` | .txt valid UTF-16 source file
`new-file-name` | .txt file to be overriden with output data
`source-file-os-flag` | `-mac`, `-unix`, `-windows` source file OS
`new-file-os-flag` | `-mac`, `-unix`, `-windows` output file OS
`byte-order-flag` | `-swap`, `-keep` indicator to switch Endians
