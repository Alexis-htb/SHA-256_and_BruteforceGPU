#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// opencl
#include <CL/cl.h>

#define rotate(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
// S0(x) = s2(x) ^ s13(x) ^ s22(x); avec sn(x) -> rotate(x,n), n = 2, n = 13, n = 22  
#define sigma0(x) (rotate(x,2) ^ rotate(x,13) ^ rotate(x, 22))
// S1(x) = s6(x) ^ s11(x) ^ s25(x); avec sn(x) -> rotate(x,n), n = 6, n = 11, n = 25  
#define sigma1(x) (rotate(x,6) ^ rotate(x, 11) ^ rotate(x,25))
// low_S0(x) = s7(x) ^ s18(x) ^ r3(x) ; avec sn(x) -> rotate(x,n), rn(x) -> x >> n, n = 7, n = 18, n = 3
#define low_sigma0(x) (rotate(x, 7) ^ rotate(x, 18) ^ (x >> 3))
// low_S1(x) = s17(x) ^ s19(x) ^ r10(x) ; avec sn(x) -> rotate(x,n), rn(x) -> x >> n, n = 17, n = 19, n = 10
#define low_sigma1(x) (rotate(x, 17) ^ rotate(x, 19) ^ (x >> 10))
// if x[i] == 1 then choose() => y[i], if x[i] == 0 then choose() => z[i]
#define choose(x,y,z) ((x & y) ^ (~x & z))
// if in x[i], y[i] and z[i] at least 2 have the value 1, then majority() => 1
#define majority(x,y,z) ((x & y) | (x & z) | (y & z))

// Fonction utilitaire pour lire le fichier .cl
char* read_kernel_source(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error : file can't be open ! %s\n", filename);
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    char* source = (char*)malloc(size + 1);
    fread(source, 1, size, file);
    source[size] = '\0';
    fclose(file);
    return source;
}

// affiche tout les combinaisons de lettres minuscules de tailles size (léger problème de logique donc quelques unes sont générées deux fois, enivron 5%)
void translateur(){
    int size = 3;
    int compteur = 0;
    //char abc[26] = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
    char chaine[size + 1];
    chaine[size] = '\0';
    int s_size = size;
    int times = 0;

    for (int i = 0; i < size; i++ ){
        if (s_size / 26 > 1){
            times += 1;
            s_size /= 26;
        }
        else{
            times += 1;
            break;
        }
    }
    int full = 0;
    chaine[0] = 'a';
    chaine[1] = 'a';
    for (int i = 1; i < size; i++){
        
        for (int j = i; j > 0; j--){  
            
            //printf("chaine %d: %s\n",compteur,chaine);                      
            if (chaine[j - 1] != 'z'){
                
                if (chaine[j] == 'z' && chaine[j - 1] != 'z'){
                    for (int k = j; k < i; k++){
                        chaine[k] = 'a';
                    }
                    
                }
                chaine[j - 1] += 1;
                full = 0;
                break;
            }
            else{   
                full = 1;
            }
        }   
        if (full == 1){
            printf("true\n");
            for (int k = 0; k < i; k++){
                chaine[k] = 'a';
            }
            //printf("chaine %d: %s\n",compteur,chaine);
            if (chaine[i] >= 'a' && chaine[i] >= 'z'){
                chaine[i] += 1;
                i--;
            }
            else{
                chaine[i] = 'a';
                
            }
        }
        else{
            i--;
        }

            
        
        full = 0;
        compteur++;
        printf("chaine %d: %s\n",compteur,chaine);
        //chaine[i+1] = '\0';
    }
}

uint64_t my_pow(int power){
    uint64_t result = 26;
    for (int i = 0; i < power; i++){
        result *= 26;
    }
    return result;
}

// transforme l'id du worker en chaine de caractere, ici le mot de passe cassé.
void id_to_password(unsigned long long id, int size) {
    char abc[] = "abcdefghijklmnopqrstuvwxyz";
    
    char chaine[32] = {0}; 

    for (int i = size - 1; i >= 0; i--) {
        chaine[i] = abc[id % 26];
        id = id / 26;
    }
    
    printf("Mot de passe genere : %s\n", chaine);
}


/*
uint32_t rotate(uint32_t toRotate, uint32_t n){
    uint32_t resultat = (toRotate >> n) | (toRotate << (32 - n));
    return resultat;
}
*/

/*
// S0(x) = s2(x) ^ s13(x) ^ s22(x); avec sn(x) -> rotate(x,n), n = 2, n = 13, n = 22  
uint32_t sigma0(uint32_t x){
    uint32_t s2 = rotate(x, 2);
    uint32_t s13 = rotate(x, 13);
    uint32_t s22 = rotate(x, 22);
    uint32_t resultat = s2 ^ s13 ^ s22;
    return resultat;
}
*/
/*
// S1(x) = s6(x) ^ s11(x) ^ s25(x); avec sn(x) -> rotate(x,n), n = 6, n = 11, n = 25  
uint32_t sigma1(uint32_t x){
    uint32_t s6 = rotate(x, 6);
    uint32_t s11 = rotate(x, 11);
    uint32_t s25 = rotate(x, 25);
    uint32_t resultat = s6 ^ s11 ^ s25;
    return resultat;
}
*/
/*
// low_S0(x) = s7(x) ^ s18(x) ^ r3(x) ; avec sn(x) -> rotate(x,n), rn(x) -> x >> n, n = 7, n = 18, n = 3
uint32_t low_sigma0(uint32_t x){
    uint32_t s7 = rotate(x, 7);
    uint32_t s18 = rotate(x, 18);
    uint32_t r3 = x >> 3;
    uint32_t resultat = s7 ^ s18 ^ r3;
    return resultat;
}
*/
/*
// low_S1(x) = s17(x) ^ s19(x) ^ r10(x) ; avec sn(x) -> rotate(x,n), rn(x) -> x >> n, n = 17, n = 19, n = 10
uint32_t low_sigma1(uint32_t x){
    uint32_t s17 = rotate(x, 17);
    uint32_t s19 = rotate(x, 19);
    uint32_t r10 = x >> 10;
    uint32_t resultat = s17 ^ s19 ^ r10;
    return resultat;
}
*/



int main(){
    char* password = "alexis";
    int size_password = 25;
    uint32_t W[64];
    

    //constantes mathématiques représentant chacune la valeur hexadécimale de la racine cubique du nombre premier k[i] 
    const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };


    printf("--- DEMARRAGE DU BRUTEFORCE GPU ---\n");

    // 1. Le hash cible du mot de passe
    uint32_t target_hash[8] = {0x8c4fd8b2, 0xc24ffcc2, 0x23dbf090, 0x88bd7973, 0x4e8404cd, 0x4d9e90fc, 0x418ecb49, 0x0622d1ca};
    int password_size = 6;
    
    
    unsigned long long result_id = 0; // 0 signifie "pas encore trouvé"

    // --- INITIALISATION OPENCL (Tuyauterie standard) ---
    cl_platform_id platform;
    clGetPlatformIDs(1, &platform, NULL);

    cl_device_id device;
    clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL); // On demande le GPU

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, NULL);

    // --- CHARGEMENT DU KERNEL ---
    char* source = read_kernel_source("bruteforce.cl");
    cl_program program = clCreateProgramWithSource(context, 1, (const char**)&source, NULL, NULL);
    clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    cl_kernel kernel = clCreateKernel(program, "bruteforce_sha256", NULL);

    // --- PREPARATION DES MEMOIRES (Envoi vers le GPU) ---
    cl_mem buffer_K = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(K), (void*)K, NULL);
    cl_mem buffer_target = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(target_hash), (void*)target_hash, NULL);
    cl_mem buffer_result = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), &result_id, NULL);

    clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_K);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_target);
    clSetKernelArg(kernel, 2, sizeof(int), &password_size);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), &buffer_result);

    // --- LE LANCEMENT DE L'USINE ! ---
    unsigned long long global_work_size = my_pow(password_size); // 26^6 => toutes les combinaisons de 6 lettres)
    printf("Lancement de %zu ouvriers...\n", global_work_size);
    
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_work_size, NULL, 0, NULL, NULL);
    clFinish(queue); // On attend que tout le monde ait fini

    // --- RECUPERATION DU RESULTAT ---
    clEnqueueReadBuffer(queue, buffer_result, CL_TRUE, 0, sizeof(int), &result_id, 0, NULL, NULL);

    if (result_id != 0) {
        printf("L'ID du mot de passe est : %d\n", result_id);
        printf("\nTraduction...\n");
        id_to_password(result_id,password_size);
    } else {
        printf("\nEchec : Mot de passe non trouve dans la plage donnee.\n");
    }


    return 0;
}
