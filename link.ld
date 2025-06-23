ENTRY(_start)
SECTIONS {
  . = 0;
  .text : { *(.text) }
  /DISCARD/ : {
    *(.note.gnu.property)
  }
}
