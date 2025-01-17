Instruções:
Para executar a aplicação, abra o main.exe
Para executar uma busca é só clicar em uma das opções na tela e escolher se quer usar 
entradas aleatórias ou carregadas de um arquivo .txt 

O arquivo .txt deve conter as entradas da seguinte forma:
1 inteiro representando a quantidade de entradas a serem executadas
para cada entrada, em uma nova linha:
    2 inteiros separados por espaço representando o ponto inicial
    2 inteiros separados por espaço representando o ponto final
    1 inteiro representando o id da função de custo(0 a 3)
    1 inteiro representando o id da função de heuristica(0 a 2)
    4 inteiros separado por espaço representando a ordem de visita dos vizinhos(combinação de [0,1,2,3])
    1 inteiro representando a quantidade de farmácias 
    para cada farmácia, em uma nova linha:
        2 inteiros separado por espaço representando a posição da farmácia

Obs: Na pasta include temos os arquivo .h que está sendo usado para declarar as funções, já na pasta src temos os arquivos .cpp
com a implementação das funções determinadas no .h, na main.cpp só tem a implementação da interface.