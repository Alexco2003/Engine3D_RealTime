#include <iostream>
#include "SceneManager.h"

int main() {
    int choice = -1;

    while (choice != 0) {
        std::cout << "\n";
        std::cout << "\n=== ENGINE 3D - SCENE MANAGER ===\n";
        std::cout << "1. Blank Window & Camera Debug\n";
        std::cout << "2. Traditional Cube Mesh\n";
        std::cout << "3. Simple Pyramid (.obj)\n";
        std::cout << "4. Complex Car Koenigsegg (.obj)\n";
        std::cout << "5. Boxman Character (.x)\n";
        std::cout << "6. House Environment (.3ds)\n";
        std::cout << "7. Sword Prop (.ms3d)\n";
        std::cout << "0. Exit Engine\n";
        std::cout << "Select a scene to run: ";

        std::cin >> choice;

        switch (choice) {
        case 1: RunSceneBlank(); break;
        case 2: RunSceneCube(); break;
        case 3: RunSceneModelPyramidOBJ(); break;
        case 4: RunSceneModelComplexOBJ(); break;
        case 5: RunSceneModelX(); break;
        case 6: RunSceneModel3DS(); break;
        case 7: RunSceneModelMS3D(); break;
        case 0: std::cout << "Exiting...\n"; break;
        default: std::cout << "Invalid choice! Try again.\n"; break;
        }
    }

    return 0;
}