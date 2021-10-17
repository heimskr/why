# Why I/O Extension

Block storage can be provided to the VM by giving it a list of files. When a program requests to perform I/O on one of the "devices," the VM will perform the corresponding operation on the file.

There's one instruction for doing IO, the IO external. Different subinstructions can be specified by providing an ID in `$a0` before calling `<io>`. Subinstructions can have their own arguments in `$a1` and beyond. Results are put into the result registers (`$r0` and beyond). IO commands are possible in ring 2 and below only. 0 is stored in `$e0` if the operation succeeded; a positive value is stored in `$e0` if the operation failed. 666 will be put in `$e0` if the value in `$a0` is invalid.

Note that `read` and `write` are incompatible with WVM's history functionality. Undoing a `read` or `write` operation won't undo the changes to the VM memory or to the file.

### `devcount` (0)

Queries the number of attached "drives."

- 0 arguments
- 1 return value: device count
- Cannot fail

### `seekabs` (1)

Seeks to an absolute position.

- 2 arguments: device ID, position to seek to
- 0 return values
- Can fail
	- 1: invalid device ID
	- 2: invalid position

### `seekrel` (2)

Seeks relative to the current position.

- 2 arguments: device ID, # bytes to skip (positive or negative)
- 1 return value: new position
- Can fail
	- 1: invalid device ID
	- 2: invalid position

### `read` (3)

Reads from a device.

- 3 arguments: device ID, address of buffer to read into, # bytes to read
- 1 return value: # bytes read
- Can fail
	- 1: invalid device ID
	- errno + 1: read failed

### `write` (4)

- 3 arguments: device ID, address of buffer to write from, # bytes to write
- 1 return value: # bytes written
- Can fail
	- 1: invalid device ID
	- errno + 1: write failed

### `getsize` (5)

Queries the capacity of a device.

- 1 argument: device ID
- 1 return value: capacity of device in bytes
- Can fail
	- 1: invalid device ID

### `getcursor` (6)

Queries a device's cursor position.

- 1 argument: device ID
- 1 return value: cursor position in bytes
- Can fail
	- 1: invalid device ID
