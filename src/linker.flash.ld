/* Based on work by Frank Duignan, modified by Sean Burford.
   http://eleceng.dit.ie/frank/arm/BareMetalXMC2Go/index.html
   useful reference: www.linuxselfhelp.com/gnu/ld/html_chapter/ld_toc.html */
MEMORY
{
	flash : org = 0x10001000, len = 64k
	ram : org = 0x20000000, len = 16k
}
  
SECTIONS
{
	. = ORIGIN(flash);
	.vectors : {
		*(.vectors)
	} >flash
	.text : {		  
		*(.text);
	} >flash
	. = ORIGIN(ram);
	.data : {	  
		*(.stext .remapped_vectors .data);
	} >ram AT>flash
	.bss : {
		*(.bss);
	} > ram
	INIT_DATA_VALUES = LOADADDR(.data);
	INIT_DATA_START = ADDR(.data);
	INIT_DATA_SIZE = SIZEOF(.data);
	BSS_START = ADDR(.bss);
	BSS_SIZE = SIZEOF(.bss);
}
