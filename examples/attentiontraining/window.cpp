#include "window.hpp"
#include "gamedata.hpp"
#include <chrono>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <imgui.h>
#include <random>

// Inicializa o jogo e configura o programa de shaders
void Window::onCreate() {
  // Carrega o shader program para renderização dos objetos
  auto const assetsPath{abcg::Application::getAssetsPath()};

  m_program = abcg::createOpenGLProgram({
      {.source = assetsPath + "/shaders/texture.vert",
       .stage = abcg::ShaderStage::Vertex},
      {.source = assetsPath + "/shaders/texture.frag",
       .stage = abcg::ShaderStage::Fragment},
  });

  // Load a new font
  auto const filename{assetsPath + "Inconsolata-Medium.ttf"};
  m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filename.c_str(), 60.0f);
  if (m_font == nullptr) {
    throw abcg::RuntimeError("Cannot load font file");
  }

  // Inicializa texturas
  m_targetTexture =
      m_targetObject.loadTexture(assetsPath + "/textures/banana.png");
  m_distractionTexture = m_distractionObjects.loadTexture(
      assetsPath + "/textures/basket_yellow.png");  

  // Configura estado inicial
  resetGame();
  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());
}

// Desenha os objetos de distração e o alvo
void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  // Renderiza objetos
  m_targetObject.paint();
  m_distractionObjects.paint();
}

// Renderiza a interface do usuário, exibindo score e vidas
void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();
  {
    auto const size{ImVec2(800, 600)};
    auto const position{ImVec2(200.0f, 1.0f)};
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags const flags{ImGuiWindowFlags_NoBackground |
                                 ImGuiWindowFlags_NoTitleBar |
                                 ImGuiWindowFlags_NoInputs};
    ImGui::Begin(" ", nullptr, flags);
    ImGui::PushFont(m_font);

    if (m_gameData.m_state == GameState::Playing) {
      ImGui::Text("Score: %d Lives: %d", m_score, m_lives);
    } else if (m_gameData.m_state == GameState::GameOver) {
      ImGui::Text("Game Over!");
      resetGame();
    } else if (m_gameData.m_state == GameState::Win) {
      ImGui::Text("*You Win!*");
      resetGame();
    } else if (m_gameData.m_state == GameState::Start) {
      ImGui::Text("Pegue Bananas!");
    } else {
      ImGui::Text("...");
    }

    ImGui::PopFont();
    ImGui::End();
  }
}

// Trata eventos de clique do mouse para interagir com os objetos
void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    glm::vec2 clickPos{
        event.button.x / static_cast<float>(m_viewportSize.x) * 2.0f - 1.0f,
        1.0f - event.button.y / static_cast<float>(m_viewportSize.y) * 2.0f};

    if (m_targetObject.checkClickOnTarget(clickPos)) {
      m_score += 1; // Ganha 5 pontos ao clicar no alvo correto
    } else if (m_distractionObjects.checkClickOnDistraction(clickPos)) {
      --m_lives; // Perde uma vida ao clicar em um objeto de distração
      updateLivesDisplay();
    }
  }
}

// Atualiza o jogo a cada frame, verifica e atualiza o status do jogo
void Window::onUpdate() {
  auto currentTime = std::chrono::steady_clock::now();
  float elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(
                          currentTime - m_lastReload)
                          .count();

  // Recarrega objetos a cada 3 segundos
  if (elapsedTime >= 2.0f && !m_gameOver) {
    --m_lives;
    m_distractionObjects.update(elapsedTime);
    updateLivesDisplay();
    m_lastReload = currentTime;
  }
  checkGameStatus();
}

// Ajusta o tamanho do viewport ao redimensionar a janela
void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

// Destroi recursos ao finalizar o jogo
void Window::onDestroy() {
  abcg::glDeleteProgram(m_program);
  m_targetObject.destroy();
  m_distractionObjects.destroy();
}

// Inicializa os objetos do jogo em posições aleatórias
void Window::initializeGameObjects() {
  // Recarrega a posição do alvo e dos objetos de distração
  m_targetObject.create(m_program, 1);
  m_distractionObjects.create(m_program, 3);
}

// Verifica o status do jogo e atualiza a flag de game over se necessário
void Window::checkGameStatus() {
  if (m_score == 0 && m_lives == 10) {
    m_gameData.m_state = GameState::Start;
  } else if (m_lives <= 0) {
    m_gameOver = true;
    m_gameData.m_state = GameState::GameOver;
  } else if (m_score >= 10) {
    m_gameOver = true;
    m_gameData.m_state = GameState::Win;
  } else {
    m_gameData.m_state = GameState::Playing;
  }
}

// Atualiza a exibição de vidas na interface do usuário
void Window::updateLivesDisplay() {
  if (m_lives <= 0) {
    m_gameOver = true;
    m_gameData.m_state = GameState::GameOver;
  }
}

// Reinicia o jogo para o estado inicial
void Window::resetGame() {
  m_score = 0;
  m_lives = 10;
  m_gameOver = false;
  m_gameData.m_state = GameState::Start; // Começar do início

  // Recria os objetos e reinicia o tempo de recarga
  m_lastReload = std::chrono::steady_clock::now();
  initializeGameObjects();
}