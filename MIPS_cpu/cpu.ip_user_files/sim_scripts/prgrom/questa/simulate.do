onbreak {quit -f}
onerror {quit -f}

vsim -lib xil_defaultlib prgrom_opt

do {wave.do}

view wave
view structure
view signals

do {prgrom.udo}

run -all

quit -force
