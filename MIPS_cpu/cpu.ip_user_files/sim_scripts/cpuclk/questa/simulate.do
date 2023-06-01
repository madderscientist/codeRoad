onbreak {quit -f}
onerror {quit -f}

vsim -lib xil_defaultlib cpuclk_opt

do {wave.do}

view wave
view structure
view signals

do {cpuclk.udo}

run -all

quit -force
