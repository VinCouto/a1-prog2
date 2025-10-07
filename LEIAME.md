------------------------------------------------------------------------
------------------------TRABALHO A1 DE PROGRAMAÇAO 2--------------------
------------------------------------------------------------------------


---------------------------------AUTORIA--------------------------------

ALUNO: VINICIUS SOUSA COUTO
GRR: 20241111


----------------------ARQUIVOS INCLUSOS NO TRABALHO---------------------

Header
  |gbv.h
  |util.h
Src
  |gbv.c
  |util.c
  |main.c
LEIAME.md
MAKEFILE
doc1.txt
doc2.txt
doc3.txt


-------------------------DIFICULDADES ENCONTRADAS-------------------------

No principio a dificuldade foi relembrar como se mexia com strings e
principalmente em se localizar na no diretorio e nos arquivos(lembrar de
como mexia nos fseek, ftell por exemplo)

Logo após essa dificuldade inicial a principal dificuldade foi contornar as
limitações impostas pelo gbv.h e pela main, esses os quais não poderiam ter
suas estruturas principais modificadas, com certeza foi a parte que mais me
tomou tempo do trabalho, principalmente nas funções em que a biblioteca não
era passada como parametro(gbv_remove ou gbv_view por exemplo), para contor
nar o problema, eu me utilizei de uma variavel global que é atualizada todo
gbv_open e que é passada para as instruções que precisam e nao a recebe 
como parâmetro.

Porém passada as limitações não me surgiu nenhuma dificuldade que vale ser
mencionada.


------------------------------BUGS CONHECIDOS-----------------------------

Após meus testes o único bug conhecido no gbv_view é

Quando adicionado um documento ao diretorio na primeira interação com o
gbv_view o mesmo não mostra nada escrito no documento, porém ao atualizar o
documento,adicionar um documento com o mesmo nome, e novamente chamar o
gbv_view, dessa vez o view mostra as coisas escritas no documento, o que
é importante ressaltar é q não precisa de fato atualizar o documento, ape
nas adicionar-lo novamente, pode ser até mesmo sem modificar nada do mesmo.


Outro bug conhecido é o gbv_list

O comando consegue ser executado independentemente da existencia ou não do
arquivo que está sendo passado na linha de comando do terminal.
Esse bug pode acabar causando uma confusão no usuário que pode tentar acessar
a lista atrás de algo e apesar do arquivo não existir, a lista ser passada 
do mesmo jeito.

O gbv_order não está implementado

Apesar de não ser obrigatorio, fiquei triste de não conseguir implementa-lo
até tentei pensar em uma ideia inicial porém nada me venho a mente e acabei
por focar nas demais funções e acabei sem tempo de implementar esta.
