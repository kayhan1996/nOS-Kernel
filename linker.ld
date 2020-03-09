ENTRY(_start)

SECTIONS
{
	. = 0xFFFFFF8000080000;
	__start = .;

	__text_start = .;
	.text : {
		KEEP(*(.text.boot))
		*(.text)
	}
	. = ALIGN(4096);
	__text_end = .;


	__rodata_start = .;
	.rodata : {
		*(.rodata);
	}
	. = ALIGN(4096);
	__rodata_end = .;

	__data_start = .;
	.data : {
		*(.data);
	}
	. = ALIGN(4096);
	__data_end = .;

	__bss_start = .;
	.bss :
	{
		*(.bss);
	}
	. = ALIGN(4096);
	__bss_end = .;

	__block_table_start = .;
	. += 0x1000 * 2;
	__block_table_end = .;

	__table_table_start = .;
	. += 0x1000;
	__table_table_end = .;

	. = ALIGN(4096);	

	__user_start__ = .;
	.user :
	{
		KEEP(*(.user));
	}
	. = ALIGN(4096);
	__user_end__ = .;

	__end = .;

}
