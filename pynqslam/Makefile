include ../build/config.mk

LIBS=../libs
include $(LIBS)/slamcommon/lib.mk

LDFLAGS:=-lsds_lib $(LDFLAGS)

EXEC_NAME = slam

include ../build/executable.mk

.PHONY: run
run: $(EXEC_NAME)
	sudo ./$(EXEC_NAME) # sudo needed for /dev/mem

#BOARD_URI=xilinx@pynq
#.PHONY: deploy
#deploy:
#	@echo ""
#	@echo "Uploading to: $(BOARD_URI)"
#	rsync -avz ./ $(BOARD_URI):~/pynqslam
#	ssh $(BOARD_URI) "cd ~/pynqslam && make"
#
#.PHONY: slamcommon
#slamcommon:
#	@echo ""
#	@echo "Uploading slam_common to: $(BOARD_URI)"
#	rsync -avz ../slam_common $(BOARD_URI):~/
#	ssh $(BOARD_URI) "cd ~/slam_common && make"
