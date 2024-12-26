# Имя выходного файла и путь к нему
TARGET = X64/VoDS

# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall \
            -I./headers \
            -I/opt/vulkan-sdk/include \
            -I/usr/include/GLFW \
            -I/usr/include/glm

# Библиотеки
LIBS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# Папка с исходниками и объектными файлами
SRC_DIR = src
OBJ_DIR = obj

# Все файлы с исходным кодом
SRCS = $(wildcard $(SRC_DIR)/*.cpp)

# Создаем список объектных файлов на основе исходных
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Цель по умолчанию, которая сначала компилирует, а потом запускает
all: $(TARGET)
	./$(TARGET)

# Цель для сборки исполняемого файла
comp: $(TARGET)

# Сборка исполняемого файла из объектных файлов
$(TARGET): $(OBJS)
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LIBS)

# Правило для компиляции объектных файлов из исходных
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Цель для запуска исполняемого файла после его компиляции
run: $(TARGET)
	./$(TARGET)

# Очистка временных файлов
clean:
	rm -rf $(OBJ_DIR) $(TARGET)