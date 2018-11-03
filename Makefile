BOARD_URI=xilinx@pynq

all: slamvis

./slamvis/slamvis:
	$(MAKE) -C slamvis

.PHONY: slamvis
slamvis: ./slamvis/slamvis

.PHONY: deploy
deploy:
	@echo ""
	@echo "Uploading to: $(BOARD_URI)"
	rsync -avz ./ $(BOARD_URI):~/slamdunk
	ssh $(BOARD_URI) "cd ~/slamdunk/pynqslam && make"
