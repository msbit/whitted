CXXFLAGS=-std=c++11 -O3
LINK.o=$(LINK.cc)
OBJFILES=light.o mesh_triangle.o object.o sphere.o vec2f.o vec3f.o whitted.o
TARGET=whitted

$(TARGET): $(OBJFILES)

clean:
	rm -f $(OBJFILES) $(TARGET)