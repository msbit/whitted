CXXFLAGS=-std=c++17 -O3 -Wall
LINK.o=$(LINK.cc)
OBJFILES=light.o mesh_triangle.o object.o sphere.o whitted.o
TARGET=whitted

$(TARGET): $(OBJFILES)

clean:
	rm -f $(OBJFILES) $(TARGET)
