OUTPUT_FORMAT("binary")
ENTRY(startup_32)
SECTIONS
{
	.text 0x0000 :
	{
		code = .; _code = .; __code = .;
		*(.text)
	}

	.data :
	{
		data = .; _data = .; __data = .;
		*(.data)
	}

	.bss :
	{
		bss = .; _bss = .; __bss = .;
		*(.bss)
	}

	end = .; _end = .; __end = .;
}
