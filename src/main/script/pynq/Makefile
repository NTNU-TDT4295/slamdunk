.PHONY: deploy cpp run nobitfile

deploy:
	@echo "Loading bitfile.."
	sudo ./load_bitfile.sh
	@echo "Compiling for CFFI.."
	sudo python3.6 test_build.py

cpp:
	@echo "Compiling for CFFI.."
	sudo python3.6 test_build.py

run:
	@echo "Running test_run.py.."
	sudo python3.6 test_run.py

nobitfile: cpp run
