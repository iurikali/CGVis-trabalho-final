# Computação Gráfica e Visualização I (INF01047) - INF/UFRGS

Este repositório contém o código base para o trabalho final. O enunciado completo do trabalho final está no Moodle:

https://moodle.ufrgs.br/mod/assign/view.php?id=6018620


Como instanciar um objeto:
- na main antes do loop principal, apenas digite new e nome da classe para chamar o construtor da classe,
o contrutor do GameObject adiciona o endereço criado nas listas certas, então não precisa associar o 
endereço a uma variável na main.
- Um objeto pode pertencer até 4 listas ao mesmo tempo, sendo estas:
    - g_collision_physics que diz se o objeto vai ter colisao física com o player
    - g_collision_triggers que diz se o objeto vai ter colisao de trigger com a hitbox do player
    - g_collision_spin que diz se o objeto vai ter colisao de trigger com a hitbox do giro do player (ataque)

    - g_non_destructible_objects objetos que não podem ser destruidos (arvores, blocos, cenários etc)
    - g_destructible_objects objetos que podem morrer e que eventualmente vão ser destruidos da memória (inimigos, frutas etc).

- A hitbox de um objeto é um componente opcional que por padrão vem desabilitado, então para habilitar tu precisa
criar a hitbox e associar ao objeto. O contrutor apenas define a geometria inicial do objeto, depois é necessário chamar a função update para posicionar a hitbox na posição correta.
    Exemplo: 
        obj->hitbox = new AABB(glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0.5, 1.0, 0.5));
        obj->hitbox->Update(g_non_destructible_objects.back()->position + glm::vec3(0.5, 0.0, 0.5));

- Os argumentos do contrutor são:
    - nome do objeto
    - obj id
    - tex id
    - se o objeto vai ter colisão física com o player
    - se o objeto vai ter colisão de trigger com o player
    - se o objeto vai ter colisão de trigger com o giro do player
    - se o objeto é morrivel


- Exemplo de instanciação:
    //Alocando espaço na memória para o objeto
    new StaticObject ("the_cube", CUBE, COBBLESTONE, true, false, false, false, glm::vec3(-1.3f, 0.0f, 0.0f));
    //Definindo a hitbox
    g_non_destructible_objects.back()->hitbox = new AABB(glm::vec3(-0.5, 0.0, -0.5), glm::vec3(0.5, 1.0, 0.5));
    g_non_destructible_objects.back()->hitbox->Update(g_non_destructible_objects.back()->position + glm::vec3(0.5, 0.0, 0.5));
