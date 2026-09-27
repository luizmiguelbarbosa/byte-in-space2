<h1 align="center">Byte in Space 2 🐶🚀💫</h1>

<p align="center">
  <b>A sequência de Byte in Space. Um jogo arcade de alta performance inspirado em Space Invaders, reescrito em C para maior velocidade e precisão.</b>
</p>

<hr>

## Desenvolvedor 🧑‍💻

<table align="center">
  <tr>
    <td align="center">
      <a href="https://github.com/luizmiguelbarbosa">
        <img src="https://avatars.githubusercontent.com/luizmiguelbarbosa" width="100px;" alt="Luiz Miguel Barbosa"/><br />
        <sub><b>Luiz Miguel Barbosa</b></sub>
      </a>
    </td>
  </tr>
</table>

<hr>

## Descrição 🌌

**Byte in Space 2** é a evolução do projeto original desenvolvido em Python. Migrando do Pygame para **C** e **Raylib**, esta sequência apresenta uma arquitetura mais robusta, melhor desempenho e recursos avançados, como shaders personalizados e compatibilidade multiplataforma. O projeto mantém a essência clássica de **Space Invaders**, enquanto explora conceitos técnicos mais avançados.

## Estrutura de Pastas 📂

O projeto segue uma estrutura modular em C para manter o código-fonte, arquivos de cabeçalho e recursos organizados:

    ├── assets
    │   ├── fonts
    │   ├── images
    │   │   └── sprites
    │   ├── ost
    │   └── shaders
    │
    ├── external
    │   ├── raylib_linux
    │   ├── raylib_macos
    │   └── raylib_windows
    │
    ├── include
    ├── src
    ├── CMakeLists.txt
    └── .idea

## Bibliotecas Utilizadas 📚

    Linguagem C
    Raylib 5.0
    CMake
    GLSL

## Distribuição das Tarefas do Projeto 🌌

<p align="center">
<table align="center">
<tr>
<th>Desenvolvedor</th>
<th>Tarefas</th>
</tr>
<tr>
<td><a href="https://github.com/luizmiguelbarbosa">Luiz Miguel Barbosa</a></td>
<td>Desenvolvimento de toda a engine do jogo em C, incluindo gerenciamento de memória, sistemas de entidades, shaders personalizados e automação da compilação multiplataforma.</td>
</tr>
</table>
</p>

## Como Executar 🚀

O projeto já possui uma versão pré-compilada para facilitar o acesso. Para executar o jogo, siga os passos abaixo:

1. **Clone o repositório:**

       git clone https://github.com/luizmiguelbarbosa/byte_in_space_2.git

2. **Acesse a pasta do executável:**

       Abra o diretório cmake-build-debug.

3. **Execute o jogo:**

       Execute o arquivo byte_in_space_2.exe.

## Conceitos Utilizados

A transição do Python para C permitiu a aplicação de conceitos mais rigorosos. O projeto passou de abstrações de alto nível para um maior controle sobre os recursos do sistema, utilizando **gerenciamento manual de memória** e **ponteiros** para otimizar o desempenho e o gerenciamento de recursos.

O uso de **structs** foi essencial para organizar os dados do jogo, servindo como base para sua arquitetura. Além disso, foram implementados **shaders personalizados (GLSL)** para aprimorar a qualidade visual, proporcionando efeitos que vão além das funções padrão de desenho.

O projeto também aplicou conceitos de **Álgebra Linear** para trabalhar com vetores de movimento e colisão, garantindo maior precisão na física do jogo e proporcionando uma experiência de controle mais responsiva em comparação à primeira versão.

## Desafios e Problemas

O maior desafio foi a transição do ambiente "gerenciado" do Python para a complexidade do gerenciamento manual de memória em C. Trabalhar sem um coletor de lixo exigiu uma abordagem muito mais disciplinada para evitar vazamentos de memória e falhas de segmentação.

Outro desafio significativo foi garantir a **compatibilidade multiplataforma**. Gerenciar diferentes binários da Raylib para Linux, macOS e Windows dentro do mesmo repositório exigiu uma compreensão mais sólida de como o CMake realiza a configuração e o vínculo de dependências externas.

Esses desafios proporcionaram uma curva de aprendizado muito mais intensa, porém também mais gratificante do que a primeira versão do projeto, demonstrando a importância de uma boa arquitetura para a construção de um software estável.

<hr>

<p align="center">
  Desenvolvido por Luiz Miguel Barbosa
</p>
```
