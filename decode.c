/*=============================================================
 * UNIFAL: UNIVERSIDADE FEDERAL DE ALFENAS
 * BACHARELADO EM CIÊNCIA DA COMPUTAÇÃO
 * Trabalho   : Decodificador do formato PGH
 * Professor  : Luiz Eduardo da Silva
 * Aluno      : Otávio Augusto Souza Martins 2022.1.08.016
 *=============================================================*/

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "imagelib.h"
 #include "huf.h"
 
 #define MAX_NODES 1024
 #define VIEWER "C:\\Program Files\\IrfanView\\i_view64.exe"


 extern struct {
     int freq, father, esquerda, direita, simbolo;
 } node[];
 
 extern int buildTree(int *hist, int size);

 
 //=================================
 // Função para erro fatal e saída
 //=================================

 void erro(const char *msg) {
     fprintf(stderr, "[ERRO] %s\n", msg);
     exit(EXIT_FAILURE);
 }

 //======================================
 // Verifica o cabeçalho do arquivo PGH
 //======================================

 void verificar_cabecalho(FILE *fp, int *rows, int *cols) {
     char linha[100];
    
     if(fgets(linha, sizeof(linha), fp) == NULL){
        printf("Erro ao ler arquivo\n");
     }

     if (linha[0] == 'P' && linha[1] == 'H'){   
        do {
            if(fgets(linha, sizeof(linha), fp) == NULL){
               printf("Erro ao ler cabeçalho!\n");
            }
        } while (linha[0] == '#');

        if(sscanf(linha, "%d %d", cols,rows) != 2){
            printf("Erro ao ler dimensões da imagem\n");
            return;
        }
     }else{
        printf("Erro, número mágico inválido!\n");
     }
 }
 
//=============================
// Lê o nível máximo de cinza
//=============================
 
 int ler_nivel(FILE *fp) {
     int nivel;
     if(fscanf(fp, "%d\n", &nivel) != 1){
        erro("Erro ao ler o nível máximo de cinza");
     }else{
     printf("[LOG] Nível máximo de cinza: %d\n", nivel);
     }
     return nivel;
 }

 //===========================
 // Lê o histograma da imagem
 //===========================

 void ler_histograma(FILE *fp, int *hist, int tamanho) {
    size_t lidos = fread(hist, sizeof(int), tamanho, fp);
    if(lidos != (size_t)tamanho){
        fprintf(stderr,  "[ERRO] Erro ao ler o histograma: esperados %d elementos, lidos %d\n", tamanho, lidos);
    }else{
        printf("[LOG] Histograma lido (%d valores)\n", tamanho);
    }
}

//===================================================
// Decodifica a imagem usando a árvore de Huffman
//total_de_pixels é a quantidade de pixels da imagem 
//====================================================

 image decodificar_imagem(FILE *fp, int cols, int rows, int maximo_cinza, int raiz) {
     int total_de_pixels = rows * cols;
     image img = img_create(rows, cols, maximo_cinza, GRAY);
     if (!img) erro("Erro ao criar imagem");
 
     unsigned char byte;
     int posicao = 0;
     int atual = raiz;
 
     while (fread(&byte, 1, 1, fp) == 1 && posicao < total_de_pixels) {
         for (int i = 7; i >= 0 && posicao < total_de_pixels; i--) {

             int bit = (byte >> i) & 1;
          
                if (bit == 1){
                    atual = node[atual].direita;
                }else{
                    atual = node[atual].esquerda;
                }
                
             if (atual < 0)
                 erro("Nó inválido durante decodificação");
 
             // Se for nó folha, grava pixel e volta para a raiz
             if (node[atual].esquerda == -1 && node[atual].direita == -1) {
                 img->px[posicao] = node[atual].simbolo;
                 posicao++;
                 atual = raiz;
             }
         }
     }
 
     if (posicao != total_de_pixels)
         fprintf(stderr, "[AVISO] Pixels esperados: %d, lidos: %d\n", total_de_pixels, posicao);
 
     return img;
 }

 //======================================================================
 // Faz a leitura completa do arquivo PGH e retorna a imagem decodificada
 //======================================================================

 image read_pgh(const char *filename) {
     printf("[LOG] Abrindo arquivo\n");
     FILE *fp = fopen(filename, "rb");
     if (!fp) erro("Falha ao abrir arquivo");

     int rows, cols;
     verificar_cabecalho(fp, &rows, &cols);
 
     int maximo_cinza = ler_nivel(fp);
 
     int hist[MAX_NODES];
     ler_histograma(fp, hist, maximo_cinza + 1);
 
     int raiz = buildTree(hist, maximo_cinza);
     printf("[LOG] Raiz da árvore de Huffman: %d\n", raiz);
 
     printf("[LOG] Decodificando imagem\n");
     image img = decodificar_imagem(fp, cols, rows, maximo_cinza, raiz);
     fclose(fp);

     printf("[LOG] Decodificação finalizada.\n");
     return img;
 }
 
 //==============================================
 // Exibe instruções de como utilizar o programa
 //==============================================

 void exibir_uso(const char *program_name) {
     printf("\nPGH Decoder\n===============================\n");
     printf("Uso: %s imagem[.pgh]\nExemplo: %s exemplo.pgh\n\n", program_name, program_name);
     exit(EXIT_FAILURE);
 }

 //===============================================
 //Função main
 //===============================================
 
 int main(int argc, char *argv[]) {
     printf("[LOG] Iniciando programa\n");
 
     if (argc < 2){
        exibir_uso(argv[0]);
     } 
 
     char base_name[100];
     strncpy(base_name, argv[1], sizeof(base_name));
     base_name[sizeof(base_name) - 1] = '\0';

     char caminhoCompleto[200];
     snprintf(caminhoCompleto,sizeof(caminhoCompleto),"imagens-pgh/%s", argv[1]);
     
 
     char input_file[100], output_file[100];
     img_name(base_name, input_file, output_file, 4, 2);

     //===============================
     // Lê e decodifica a imagem PGH
     //===============================

     image img = read_pgh(caminhoCompleto);
 
     printf("[LOG] Salvando imagem\n");
     img_put(img, output_file, GRAY);
     printf("[LOG] Imagem salva: %s\n", output_file);

     //==================================================
     // Monta comando para abrir visualizador de imagens
     //==================================================

     char cmd[150];
 #if defined(_WIN32) || defined(__WIN64__) || defined(__CYGWIN__)
     
    printf("[LOG] Executando visualizador:\n");
    sprintf(cmd, "start \"\" \"%s\" \"%s\"", VIEWER, output_file);  
    system(cmd);
    printf("[LOG] Encerrando programa.\n");
 #else
     printf("[LOG] Executando visualizador:\n ")
     snprintf(cmd, sizeof(cmd), "eog %s &", output_file);
     system(cmd);
     printf("[LOG] Encerrando programa.\n");
 #endif
     printf("[LOG] Programa encerrado com sucesso!\n");
     img_free(img);
     
     return 0;
 }
 