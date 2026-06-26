# Relatório
Nós, alunos Iuri Kali e Rafael Alexandrini, recriamos (de forma simplificada) a fase inicial do jogo Crash Bandicoot, do Playstation 1. 

Nossa versão possui os movimentos do Crash (andar, pular e girar), a câmera 'em trilho' tradicional do jogo, uma câmera em primeira pessoa, caixas, frutas coletáveis, inimigos e espinhos que forçam o jogador a reiniciar o jogo. 

Testes de colisão são feitos usando hitboxes AABB. A iluminação é uma mistura do modelo de Phong - com parte especular e glossy - com uma iluminação difusa do modelo [Half-Lambert](https://developer.valvesoftware.com/wiki/Half_Lambert), já usado em jogos da Valve, como Half-Life. A instanciação dos objetos no mapa é feito por meio de um arquivo JSON na pasta /src, onde são definidos prefabs e onde são instanciados.

## Controles
WASD ou Setinhas para movimentação
Ctrl ou J para girar
P para alternar entre câmera de primeira e terceira pessoa
Mouse para mover a câmera de primeira pessoa
R para recarregar o mapa (armazenado em src/mapa1.json)

## Compilação

Passos presentes em COMPILACAO.md

Forma mais comum: com a extensão do CMake no VSCode, aperte no botão Build ou Launch na parte inferior da tela.
O executável fica em /bin/Debug/main.exe.

## Imagens
![Visão inicial do jogo]({50B8FDE0-8EE3-4EEB-94F9-542DF869AC8D}.png)
![Crash quebrando uma caixa. Brilho das frutas]({31A7A41B-E224-48AC-9293-BE66487CEB5B}.png)
![Visão em primeira pessoa]({44247B12-F9E7-44BB-8C49-E36120EDB0BD}.png)

## Contribuição dos membros da dupla
Iuri:

Animação do personagem; Colisão; Física; Inimigos; Player; Espinhos; Partículas; Câmera em primeira pessoa;

Rafael:

Carregamento e criação do Mapa; Modelo de Iluminação; Câmeras; Movimentação do inimigo; Mapeamento de texturas;

## Uso de IA Generativa
Rafael: 

Usei principalmente Claude como forma de tirar dúvidas e gerar ideias. Por exemplo, o sistema de mapa em JSON foi sugerido por IA, mas a lógica implementada foi minha. Em geral, não usei para a escrita de código diretamente. 
Para tirar dúvidas achei bem útil, já que oferece respostas já no contexto específico deste projeto. Porém, não é perfeito: tentei usar para debugar certos erros e não conseguiu ajudar.

Iuri:

<mark> completar </mark>