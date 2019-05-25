all: .SILENT

.SILENT:
	cmake --build bin-release -- -j10
