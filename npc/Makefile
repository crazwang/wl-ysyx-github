MODULE = top

all: ./obj_dir/V$(MODULE)
	@echo " ### executing file V$(MODULE) ### "
	./obj_dir/Vtop

./obj_dir/V$(MODULE): sim

wave: 
	@echo " ### use gtkwave to view the waveform ###"
	gtkwave waveform.vcd

.PHONY:run
run:
	@echo "### run the file /obj_dir/V$(MODULE) ###"
	@./obj_dir/V$(MODULE)
	
.PHONY: sim
sim:
	$(call git_commit, "sim RTL") # DO NOT REMOVE THIS LINE!!!
	@echo " ### Verilator Simulating Begin ###  "
	verilator -Wall --trace --cc --exe --build csrc/main.cpp vsrc/$(MODULE).v

#@echo "Write this Makefile by your self."


.PHONY:clean
clean:
	rm *.vcd
	rm -rf obj_dir

include ../Makefile
