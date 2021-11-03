# Why I/O Extension

Block storage can be provided to the VM by giving it a list of files. When a program requests to perform I/O on one of the "devices," the VM will perform the corresponding operation on the file.

There's one instruction for doing IO, the IO external. Different subinstructions can be specified by providing an ID in `$a0` before calling `<io>`. Subinstructions can have their own arguments in `$a1` and beyond. Results are put into the result registers (`$r0` and beyond). IO commands are possible in ring 2 and below only. 0 is stored in `$e0` if the operation succeeded; a positive value is stored in `$e0` if the operation failed. 666 will be put in `$e0` if the value in `$a0` is invalid.

Note that `read` and `write` are incompatible with WVM's history functionality. Undoing a `read` or `write` operation won't undo the changes to the VM memory or to the file.

### `devcount` (0)

Queries the number of attached drives.

- No arguments
- 1 return value: device count
- Cannot fail

### `seekabs` (1)

Seeks to an absolute position.

- 2 arguments:
	1. Device ID
	2. Position to seek to
- 0 return values
- Can fail
	- 1: Invalid device ID
	- 2: Invalid position

### `seekrel` (2)

Seeks relative to the current position.

- 2 arguments:
	1. Device ID
	2. \# bytes to skip (positive or negative)
- 1 return value: new position
- Can fail
	- 1: Invalid device ID
	- 2: Invalid position

### `read` (3)

Reads from a device.

- 3 arguments:
	1. Device ID
	2. Address of buffer to read into
	3. \# bytes to read
- 1 return value: # bytes read
- Can fail
	- 1: Invalid device ID
	- 2 & PFAULT: A page fault occurred while writing to the buffer
	- 3 & BWRITE: The page containing part of the buffer is unwritable
	- errno + 3: Read failed

### `write` (4)

Writes to a device.

- 3 arguments:
	1. Device ID
	2. Address of buffer to write from
	3. \# bytes to write
- 1 return value: # bytes written
- Can fail
	- 1: Invalid device ID
	- 2 & PFAULT: A page fault occurred while reading from the buffer
	- errno + 2: Write failed

### `getsize` (5)

Queries the capacity of a device.

- 1 argument: device ID
- 1 return value: capacity of device in bytes
- Can fail
	- 1: Invalid device ID
	- errno + 1: Couldn't find size

### `getcursor` (6)

Queries a device's cursor position.

- 1 argument: device ID
- 1 return value: cursor position in bytes
- Can fail
	- 1: Invalid device ID
	- errno + 1: Couldn't find cursor

### `getname` (7)

Queries a device's name.

- 3 arguments:
	1. Device ID
	2. Address of buffer to store device name in
	3. Size of buffer in bytes
- 1 return value: number of bytes in the name (excluding null terminator)
- Can fail
	- 1: Invalid device ID
	- 2 + PFAULT: A page fault occurred while writing to the buffer
	- 3 + BWRITE: The page containing part of the buffer is unwritable