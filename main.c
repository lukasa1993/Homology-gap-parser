//
// Created by lukasa on 11/20/15.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "complex_struct.h"
#include "file_util.h"

char *file_to_string(char *filename);

int CalculatePoints(Complex *comp);

int main(int argc, char *argv[]) {

    if (argc != 2) {
        return -1;
    }

    char *file_content = file_to_string(argv[1]);
    size_t file_length = strlen(file_content);

    char literal[2048];
    int literal_i = 0;

    int line = 0;

    Complex *A, *B;
    int bPoints;

    LD_File *file = Init_file_util_ext("./hom_result", "txt", false);

    wrtieLine(file, "RequirePackage(\"homology\");", false);
    wrtieLine(file, "complex := [];", false);

    for (int i = 0; i < file_length; i++) {
        char aChar = file_content[i];
        if (aChar == '\n') {
            line++;

            if (line == 1) {
                const char *AL = strtok(literal, "->");
                const char *BL = strtok(NULL, "->");

                A = literalToComplex((char *) AL);
                B = literalToComplex((char *) BL);

                bPoints = CalculatePoints(B);

                memset(literal, 0, literal_i + 10);
                literal_i = 0;
                continue;
            }

            Complex *comp = literalToComplex(literal);

            if (comp != NULL) {
                Complex *P = literalToComplex(literal);
                Simplex *tmp = Init_Simplex();

                for (int f = 0; f < P->simplexCount; ++f) {
                    Simplex *simp = getSimpexAt(P, f);
                    for (int j = 0; j < simp->elementCount; ++j) {
                        addElement(tmp, ((f * bPoints) + getElementAt(simp, j)));
                    }
                }


                char stmp[1000];
                sprintf(stmp, "Append(complex, %s);", simplexToLiteral(tmp));

                wrtieLine(file, stmp, false);
                Dest_Complex(P);
            }


            memset(literal, 0, literal_i + 10);
            literal_i = 0;
        } else {
            literal[literal_i++] = aChar;
        }
    }
    wrtieLine(file, "SimplicialHomology(OrderComplex(OrderRelationToPoset(complex,IsSubset)));", true);

    printf("\nResult is in:\n %s\n", file->path);

    Destroy_file(file);

}

char *file_to_string(char *filename) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL) {
        char error[255];
        sprintf(error, "Error while opening %s. \n", filename);
        perror(error);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    size_t fsize = (size_t) ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;

    return string;
}


int CalculatePoints(Complex *comp) {
    SimplexElem elem = -1;

    for (int i = 0; i < comp->simplexCount; ++i) {
        Simplex *simp = getSimpexAt(comp, i);
        for (int j = 0; j < simp->elementCount; ++j) {
            SimplexElem elemMax = getElementAt(simp, j);
            if (elemMax > elem) {
                elem = elemMax;
            }
        }
    }
    return elem;
}
