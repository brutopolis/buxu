#include "buxu.h"

// bpm - Buxu Package Manager

#define BPM_VERSION "0.0.1"

function(_package)//defines the current package name
{
    if (args->size != 1)
    {
        buxu_error("bpm.package: invalid number of arguments");
        return -1;
    }
    char* name = arg(0).s;
    Int index = hash_find(vm, "package");
    free(data(index).s);
    data(index).s = str_duplicate(name);
    return -1;
}

function(_file)
{
    Int repoindex = hash_find(vm, "bpm.repo");
    if (args->size != 1)
    {
        buxu_error("bpm.file: invalid number of arguments");
        return -1;
    }
    char* repo = data(repoindex).s;
    char* filename = arg(0).s;
    char* command = str_format("wget %s/%s", repo, filename);
}

function(_set_repo)
{
    if (args->size != 1)
    {
        buxu_error("bpm.repo.add: invalid number of arguments");
        return -1;
    }

    char* repo = arg(0).s;
    Int index = hash_find(vm, "bpm.repo");
    free(data(index).s);
    data(index).s = str_duplicate(repo);
    return -1;
}

function(_install)
{
    Int index = hash_find(vm, "bpm.repo");
    char* name = arg(0).s;
    char* repo = data(index).s;
    char* command = str_format("mkdir -p ./.buxu/bpm/%s", name);
    system(command);
    free(command);
    command = str_format("cd ./.buxu/bpm/%s", name);
    system(command);
    free(command);
    command = str_format("curl -LO %s/%s/package.br", repo, name);
    system(command);
    free(command);

    char* filename = str_format("./.buxu/bpm/%s/package.br", name);
    char* code = readfile(filename);
    if (code == NULL)
    {
        buxu_error("bpm.install: package %s not found", name);
        return -1;
    }
    Int result = eval(vm, code);
    free(code);
    if (result == -1)
    {
        buxu_error("bpm.install: error while installing package %s", name);
        return -1;
    }

}

init(bpm)
{
    Int index = new_first_var(vm, "bpm.repo");
    data(index).s = str_duplicate("jardimdanificado.github.io/bpm_packages");
    
    index = new_first_var(vm, "bpm.version");
    data(index).s = str_duplicate(BPM_VERSION);

    add_function(vm, "repo.set", _set_repo);
}