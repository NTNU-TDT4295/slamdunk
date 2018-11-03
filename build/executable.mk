BUILD_DIR=./build
SRC_DIR=./src

SRC:=$(shell find $(SRC_DIR)/ -name *.cpp)
OBJS:=$(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

all: $(EXEC_NAME)

$(EXEC_NAME): $(OBJS) $(LOCAL_DEPENDENCIES)
	echo $(LOCAL_DEPENDENCIES)
	$(CXX) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	-rm -f $(OBJS)
	-rm -f $(EXEC_NAME)
