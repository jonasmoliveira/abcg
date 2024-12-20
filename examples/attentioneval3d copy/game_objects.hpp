#ifndef GAME_OBJECTS_HPP_
#define GAME_OBJECTS_HPP_

#include "abcgOpenGL.hpp"
// #include "gamedata.hpp"
#include "model.hpp"
#include <glm/vec2.hpp>
#include <list>
#include <random>
#include <string>
#include <vector>

// Dados de posição do vértice
struct PositionVertex {
  glm::vec3 position{};

  friend bool operator==(PositionVertex const &,
                         PositionVertex const &) = default;
};

// Classe que representa os objetos do jogo
class GameEntities {

public:
  // Estrutura de um objeto de cena
  struct SceneObject {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };

  // Estrutura de um objeto do jogo
  struct GameObject {
    float m_angularVelocity{};
    float m_rotation{0.0f};
    float m_scale{};
    bool checkClick(glm::vec3 const &clickPos) const;
  };

  // Vetores de objetos de cena
  std::vector<SceneObject> m_distractionObjects = std::vector<SceneObject>(20);
  std::vector<SceneObject> m_targetObjects = std::vector<SceneObject>(30);

  // Número de triângulos
  [[nodiscard]] int getNumTriangles() const {
    return gsl::narrow<int>(m_indices.size()) / 3;
  }

  // Funções de ciclo de vida do renderização
  void create();
  void paint();
  void paintUI();
  void destroy();
  void update(float deltaTime);
  void render(int numTriangles = -1) const;
  void setupVAO(GLuint program);
  bool isEmpty() const;

  // Funções de manipulação dos modelos
  void setModelVariables(const GLint KaLoc, const GLint KdLoc,
                         const GLint KsLoc, const GLint shininessLoc,
                         const GLint modelMatrixLoc);

  // Funções de manipulação de objetos da cena
  void updateSceneObjects(std::vector<SceneObject> &m_st, float deltaTime,
                          float incZ, float incX, float incY, float posZ,
                          float minX, float maxX, float minY, float maxY,
                          float minZ, float maxZ);
  void setupSceneObjects(std::vector<SceneObject> &m_sceneObjects, float minX,
                         float maxX, float minY, float maxY, float minZ,
                         float maxZ);
  void createObject(Model &m_model, const std::string &assetsPath,
                    const std::string &objPath,
                    const std::string &texturePaths);
  void renderObject(Model &m_model, const GLint KaLoc, const GLint KdLoc,
                    const GLint KsLoc, const GLint shininessLoc,
                    const GLint modelMatrixLoc,
                    std::vector<SceneObject> &m_sceneObjects);
  void randomizeSceneObject(SceneObject &sceneObject, float minX, float maxX,
                            float minY, float maxY, float minZ, float maxZ);
  void updateObjects(float deltaTime);
  bool checkClickOnObject(glm::vec3 const &clickPos,
                          glm::mat4 const &viewMatrix,
                          glm::mat4 const &projMatrix,
                          std::vector<SceneObject> &m_sceneObjects);
  void removeObject(std::list<GameObject>::iterator it);
  void loadObj(std::string_view path, bool standardize = true);

  // Definição matrizes de projeção e visão
  glm::ivec2 m_viewportSize{};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};

  // Variáveis
  std::default_random_engine m_randomEngine;
  float m_FOV{30.0f};

private:
  // Modelos
  Model m_distractionModel;
  Model m_targetModel;

  // Lista de objetos do jogo
  std::list<GameObject> m_distractions;
  std::list<GameObject> m_targets;

  // Programa de shader
  GLuint m_program{};
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  // Variáveis de shader
  GLint m_translationLoc{};
  GLint m_scaleLoc{};

  // Variáveis de modelo (propriedades do material)
  glm::vec4 m_lightDir{-1.0f, -1.0f, -1.0f, 0.0f};
  glm::vec4 m_Ia{1.0f};
  glm::vec4 m_Id{1.0f};
  glm::vec4 m_Is{1.0f};
  glm::vec4 m_Ka{};
  glm::vec4 m_Kd{};
  glm::vec4 m_Ks{};
  float m_shininess{};
  float m_angle{};

  // Variáveis de controle distribuição de objetos
  std::uniform_real_distribution<float> m_randomDist{-1.0f, 1.0f};
  std::vector<PositionVertex> m_vertices;
  std::vector<GLuint> m_indices;
};

#endif
