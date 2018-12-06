CXX := clang++
CXXFLAGS := -std=c++17 -g
LDFLAGS := 

PKGS := egl harfbuzz freetype2 glesv2 gtk+-3.0 icu-i18n
PKG_CXXFLAGS := $(shell pkg-config $(PKGS) --cflags)
PKG_LIBS = $(shell pkg-config $(PKGS) --libs)

SRC_DIR = ./src
OBJ_DIR = ./obj
SRCS = $(wildcard $(SRC_DIR)/*.cc)
OBJS = $(addprefix $(OBJ_DIR)/, $(notdir $(SRCS:.cc=.o)))
DEPS = $(OBJS:.o=.d)

TARGET = editor

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(PKG_LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@-mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(PKG_CXXFLAGS) -o $@ -c -MMD -MP $<

all: clean $(TARGET)

clean:
	@-rm -f $(OBJS) $(DEPS) $(TARGET)

run: $(TARGET)
	./editor -w 1280x1280 -p 48 -d -v

debug: $(TARGET)
	gdb --args ./editor -w 1280x512 -p 64 -d -v


-include $(DEPS)
