function init(types)

    local starter = [[#include <stdio.h>
    #include <stdlib.h>    
    #include <stdbool.h>
    
    typedef char int8;
    typedef short int16;
    typedef int int32;
    typedef long int64;
    typedef unsigned char uint8;
    typedef unsigned short uint16;
    typedef unsigned int uint32;
    typedef unsigned long uint64;
    ]]
    
    for k,v in ipairs(types) do
        starter = starter .. [[
            typedef struct {
                ]] .. v .. [[* array;
                size_t size;
            } List_]] .. v .. [[;
            
            // Função para inicializar uma lista vazia
            List_]] .. v .. [[* list_]] .. v .. [[_initialize() {
                List_]] .. v .. [[* list = (List_]] .. v .. [[*)malloc(sizeof(List_]] .. v .. [[));
                if (list == NULL) {
                    fprintf(stderr, "Erro: Falha na alocação de memória.\n");
                    exit(EXIT_FAILURE);
                }
            
                list->array = NULL;
                list->size = 0;
                return list;
            }
            
            // Função para adicionar um elemento em um índice específico
            void list_]] .. v .. [[_insert(List_]] .. v .. [[* list, ]] .. v .. [[ element, size_t index) {
                if (index > list->size) {
                    fprintf(stderr, "Erro: Índice fora do alcance da lista.\n");
                    exit(EXIT_FAILURE);
                }
            
                // Realloc para aumentar o tamanho da lista
                list->array = (]] .. v .. [[*)realloc(list->array, (list->size + 1) * sizeof(]] .. v .. [[));
                if (list->array == NULL) {
                    fprintf(stderr, "Erro: Falha na alocação de memória.\n");
                    exit(EXIT_FAILURE);
                }
            
                // Desloca os elementos para abrir espaço para o novo elemento
                for (size_t i = list->size; i > index; i--) {
                    list->array[i] = list->array[i - 1];
                }
            
                // Insere o novo elemento no índice especificado
                list->array[index] = element;
                list->size++;
            }
            
            // Função para remover um elemento em um índice específico
            void list_]] .. v .. [[_remove(List_]] .. v .. [[* list, size_t index) {
                if (index >= list->size) {
                    fprintf(stderr, "Erro: Índice fora do alcance da lista.\n");
                    exit(EXIT_FAILURE);
                }
            
                // Desloca os elementos para cobrir o elemento a ser removido
                for (size_t i = index; i < list->size - 1; i++) {
                    list->array[i] = list->array[i + 1];
                }
            
                // Realloc para reduzir o tamanho da lista
                list->array = (]] .. v .. [[*)realloc(list->array, (list->size - 1) * sizeof(]] .. v .. [[));
                if (list->size > 1 && list->array == NULL) {
                    fprintf(stderr, "Erro: Falha na alocação de memória.\n");
                    exit(EXIT_FAILURE);
                }
            
                list->size--;
            }
            
            // Função para obter o tamanho da lista
            size_t list_]] .. v .. [[_size(List_]] .. v .. [[* list) {
                return list->size;
            }
            
            // Função para liberar a memória alocada para a lista
            void list_]] .. v .. [[_free(List_]] .. v .. [[* list) {
                free(list->array);
                free(list);
            }

            ]] .. v .. [[* list_]] .. v .. [[_get(List_]] .. v .. [[* list, size_t index) {
                if (index >= list->size) {
                    fprintf(stderr, "Erro: Índice fora do alcance da lista.\n");
                    exit(EXIT_FAILURE);
                }
                return &list->array[index];
            }

            void list_]] .. v .. [[_set(List_]] .. v .. [[* list, size_t index, ]] .. v .. [[ element) {
                if (index >= list->size) {
                    fprintf(stderr, "Erro: Índice fora do alcance da lista.\n");
                    exit(EXIT_FAILURE);
                }
                list->array[index] = element;
            }

            void list_]] .. v .. [[_resize(List_]] .. v .. [[* list, size_t size) {
                list->array = (]] .. v .. [[*)realloc(list->array, size * sizeof(]] .. v .. [[));
                if (list->array == NULL) {
                    fprintf(stderr, "Erro: Falha na alocação de memória.\n");
                    exit(EXIT_FAILURE);
                }
                list->size = size;
            }

            void list_]] .. v .. [[_concat(List_]] .. v .. [[* list1, List_]] .. v .. [[* list2) {
                list1->array = (]] .. v .. [[*)realloc(list1->array, (list1->size + list2->size) * sizeof(]] .. v .. [[));
                if (list1->array == NULL) {
                    fprintf(stderr, "Erro: Falha na alocação de memória.\n");
                    exit(EXIT_FAILURE);
                }
                for (size_t i = 0; i < list2->size; i++) {
                    list1->array[list1->size + i] = list2->array[i];
                }
                list1->size += list2->size;
            }
        ]]
    end
    
    local C_clists = terralib.includecstring(starter)
    
    local clist = {type={}}
    
    for k,v in ipairs(types) do
        -- funções temporarias para cada tipo
        local tempFunctions = {}
        tempFunctions.initialize = C_clists["list_" .. v .. "_initialize"];
        tempFunctions.insert = C_clists["list_" .. v .. "_insert"];
        tempFunctions.remove = C_clists["list_" .. v .. "_remove"];
        tempFunctions.size = C_clists["list_" .. v .. "_size"];
        tempFunctions.free = C_clists["list_" .. v .. "_free"];
        tempFunctions.get = C_clists["list_" .. v .. "_get"];
        tempFunctions.set = C_clists["list_" .. v .. "_set"];
        tempFunctions.resize = C_clists["list_" .. v .. "_resize"];
        tempFunctions.concat = C_clists["list_" .. v .. "_concat"];

        -- etc
        local temptype = terralib.loadstring("return " .. v)();
        local ltype = &C_clists["List_" .. v]
        clist.type[v] = C_clists["List_" .. v]

        --local __struct = terralib.loadstring("return(struct{list: @" .. v .. "})")();
        local struct __struct {
            list: ltype
        }

        -- funções para manipular a lista
        terra __struct:new()
            var result:__struct = [__struct]{list=tempFunctions.initialize()}
            return result
        end
        terra __struct:insert(element: temptype, index: int32)
            tempFunctions.insert(self.list, element, index)
        end
        terra __struct:push(element: temptype)
            tempFunctions.insert(self.list, element, tempFunctions.size(self.list))
        end
        terra __struct:remove(index: int)
            tempFunctions.remove(self.list, index)
        end
        terra __struct:size()
            return tempFunctions.size(self.list)
        end
        terra __struct:free()
            tempFunctions.free(self.list)
        end
        terra __struct:get(index: int)
            return @tempFunctions.get(self.list, index)
        end
        terra __struct:set(index: int, element: temptype)
            tempFunctions.set(self.list, index, element)
        end
        terra __struct:resize(size: int)
            tempFunctions.resize(self.list, size)
        end
        terra __struct:concat(list: __struct)
            tempFunctions.concat(self.list, list.list)
        end

        clist[v] = {}
        
        clist[v].insert = terra(list:__struct, element: temptype, index: int32)
            tempFunctions.insert(list.list, element, index)
        end
        clist[v].push = terra(list:__struct, element: temptype)
            tempFunctions.insert(list.list, element, tempFunctions.size(list.list))
        end
        clist[v].remove = terra(list:__struct, index: int)
            tempFunctions.remove(list.list, index)
        end
        clist[v].size = terra(list:__struct)
            return tempFunctions.size(list.list)
        end
        clist[v].free = terra(list:__struct)
            tempFunctions.free(list.list)
        end
        clist[v].get = terra(list:__struct, index: int)
            return @tempFunctions.get(list.list, index)
        end
        clist[v].new = terra()
            var result:__struct = [__struct]{list=tempFunctions.initialize()}
            return result
        end
        clist[v].set = terra(list:__struct, index: int, element: temptype)
            tempFunctions.set(list.list, index, element)
        end
        clist[v].resize = terra(list:__struct, size: int)
            tempFunctions.resize(list.list, size)
        end
        clist[v].concat = terra(list1:__struct, list2: __struct)
            tempFunctions.concat(list1.list, list2.list)
        end
        
    end
    
    return clist

end

return init