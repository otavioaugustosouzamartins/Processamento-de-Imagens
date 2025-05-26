/*=============================================================
 * UNIFAL = UNIVERSIDADE FEDERAL DE ALFENAS
 * BACHARELADO EM CIÊNCIA DA COMPUTAÇÃO
 * Trabalho . . : Decodificador do formato PGH
 * Professor . : Luiz Eduardo da Silva
 * Aluno . . . : Otávio Augusto Souza Martins 2022.1.08.016
 *=============================================================*/

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include "imagelib.h"
 #include "huf.h"
 
 #define MAX_NODES 1024
 #define VIEWER "C:\\Program Files\\IrfanView\\i_view64.exe"


 
 extern struct {
     int freq, father, left, right, symbol;
 } node[];
 
 extern int buildTree(int *hist, int size);
 
 // Função para imprimir log
 void log_msg(const char *msg) {
     printf("[LOG] %s\n", msg);
     fflush(stdout);
 }
 
 // Função para erro fatal e saída
 void erro(const char *msg) {
     fprintf(stderr, "[ERRO] %s\n", msg);
     exit(EXIT_FAILURE);
 }
 
 // Verifica o cabeçalho do arquivo PGH
 void verificar_cabecalho(FILE *fp) {
     char linha[100];
     fgets(linha, sizeof(linha), fp);
     if (strncmp(linha, "PH", 2) != 0)
         erro("Número mágico inválido (esperado: PH)");
 
     // Pula linhas de comentário iniciadas por '#'
     do {
         fgets(linha, sizeof(linha), fp);
     } while (linha[0] == '#');
 
     // Retrocede para a linha lida, que contém as dimensões
     fseek(fp, -strlen(linha), SEEK_CUR);
 }
 
 // Lê as dimensões da imagem (colunas x linhas)
 void ler_dimensoes(FILE *fp, int *cols, int *rows) {
     fscanf(fp, "%d %d\n", cols, rows);
     printf("[LOG] Dimensões: %d x %d\n", *cols, *rows);
 }
 
 // Lê o nível máximo de cinza
 int ler_nivel(FILE *fp) {
     int nivel;
     fscanf(fp, "%d\n", &nivel);
     printf("[LOG] Nível máximo de cinza: %d\n", nivel);
     return nivel;
 }
 
 // Lê o histograma da imagem
 void ler_histograma(FILE *fp, int *hist, int tamanho) {
     fread(hist, sizeof(int), tamanho, fp);
     printf("[LOG] Histograma lido (%d valores)\n", tamanho);
 }
 
 // Decodifica a imagem usando a árvore de Huffman
 image decodificar_imagem(FILE *fp, int cols, int rows, int max_gray, int root) {
     int total_pixels = rows * cols;
     image img = img_create(rows, cols, max_gray, GRAY);
     if (!img) erro("Erro ao criar imagem");
 
     unsigned char byte;
     int pos = 0;
     int current = root;
 
     while (fread(&byte, 1, 1, fp) == 1 && pos < total_pixels) {
         for (int i = 7; i >= 0 && pos < total_pixels; i--) {
             int bit = (byte >> i) & 1;
             current = (bit == 0) ? node[current].left : node[current].right;
 
             if (current < 0)
                 erro("Nó inválido durante decodificação");
 
             // Se nó folha, grava pixel e volta para a raiz
             if (node[current].left == -1 && node[current].right == -1) {
                 img->px[pos++] = node[current].symbol;
                 current = root;
             }
         }
     }
 
     if (pos != total_pixels)
         fprintf(stderr, "[AVISO] Pixels esperados: %d, lidos: %d\n", total_pixels, pos);
 
     return img;
 }
 
 // Remove a extensão ".pgh" e retorna nome base (sem extensão)
 void remover_extensao_pgh(char *filename) {
     char *dot = strrchr(filename, '.');
     if (dot && strcmp(dot, ".pgh") == 0) {
         *dot = '\0';  // Remove a extensão .pgh
     }
 }
 
 // Cria o nome de entrada e saída baseado no nome base da imagem
 void criar_nomes(const char *base_name, char *in_name, char *out_name) {
     // Input filename com extensão .pgh
     snprintf(in_name, 100, "%s.pgh", base_name);
     // Output filename com sufixo "-result" e extensão .pgm
     snprintf(out_name, 100, "%s-result.pgm", base_name);
 }
 
 // Faz a leitura completa do arquivo PGH e retorna a imagem decodificada
 image read_pgh(const char *filename) {
     log_msg("Abrindo arquivo...");
     FILE *fp = fopen(filename, "rb");
     if (!fp) erro("Falha ao abrir arquivo");
 
     verificar_cabecalho(fp);
 
     int cols, rows;
     ler_dimensoes(fp, &cols, &rows);
     int max_gray = ler_nivel(fp);
 
     int hist[MAX_NODES];
     ler_histograma(fp, hist, max_gray + 1);
 
     int root = buildTree(hist, max_gray);
     printf("[LOG] Raiz da árvore de Huffman: %d\n", root);
 
     log_msg("Decodificando imagem...");
     image img = decodificar_imagem(fp, cols, rows, max_gray, root);
     fclose(fp);
     log_msg("Decodificação finalizada.");
     return img;
 }
 
 void exibir_uso(const char *program_name) {
     printf("\nPGH Decoder\n===============================\n");
     printf("Uso: %s imagem[.pgh]\nExemplo: %s exemplo.pgh\n\n", program_name, program_name);
     exit(EXIT_FAILURE);
 }
 
 int main(int argc, char *argv[]) {
     log_msg("Iniciando programa...");
 
     if (argc < 2) exibir_uso(argv[0]);
 
     char base_name[100];
     strncpy(base_name, argv[1], sizeof(base_name));
     base_name[sizeof(base_name) - 1] = '\0';

     char caminhoCompleto[200];
     snprintf(caminhoCompleto,sizeof(caminhoCompleto),"imagens-pgh/%s", argv[1]);
     
 
     // Remove extensão .pgh, se existir
     remover_extensao_pgh(base_name);
 
     char input_file[100], output_file[100];
     criar_nomes(base_name, input_file, output_file);
 
     // Lê e decodifica a imagem PGH
     image img = read_pgh(caminhoCompleto);
     //image img = read_pgh(input_file);
 
     log_msg("Salvando imagem...");
     img_put(img, output_file, GRAY);
     printf("[LOG] Imagem salva: %s\n", output_file);
 
     // Monta comando para abrir visualizador de imagens
     char cmd[150];
 #if defined(_WIN32) || defined(__WIN64__) || defined(__CYGWIN__)
     
    log_msg("Executando visualizador...");
    sprintf(cmd, "start \"\" \"%s\" \"%s\"", VIEWER, output_file);  
    system(cmd);
    log_msg("Encerrando programa.");
 #else
     snprintf(cmd, sizeof(cmd), "eog %s &", output_file);
     system(cmd);
     log_msg("Encerrando programa.");
 #endif
     log_msg("Programa encerrado com sucesso!");
     img_free(img);
     
     return 0;
 }
 