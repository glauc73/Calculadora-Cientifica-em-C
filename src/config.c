#include "config.h"
#include "safeget.h"

program_config settings;

void get_config_file(){
	FILE* config_file = fopen(CONFIG_FILE_NAME, "rb");
	//no caso de não possuir arquivo criado, crie e inicia tudo em default
	if(!config_file) {
		fclose(config_file);
		config_file = fopen(CONFIG_FILE_NAME, "wb");
		settings.angle = RAD;
		settings.range = DEFAULT_RANGE;
		fwrite(&settings, sizeof (program_config), 1, config_file);
	}else {
		fread(&settings, sizeof(program_config), 1, config_file);  //caso o arquivo exista, adiciona o conteudo de settings no arquivo
	}
	fclose(config_file);
}

void save_config_file() {
	FILE* config_file = fopen(CONFIG_FILE_NAME, "wb");
	if(config_file == NULL) {
		puts("falha ao abrir arquivo\n");
	}
	fwrite(&settings, sizeof(program_config), 1, config_file);
	fclose(config_file);
}

void select_config(int choice) {
	switch (choice)
	{
	case 1:
		printf
		(
			"Informe o indice da unidade desejada\n"
			"1) Radiano\n"
			"2) Grau\n"
		);
		unsigned um_angle = get_u64_limited(1, 2, "");
		settings.angle = (um_angle == 1) ? RAD : DEGREE; 
		break;
	case 2:
		printf
		(
			"informe o intervalo de precisao desejado:\n"
			"(os candidatos iniciais do metodo de newton estao em [-a, a])\n"
		);
		settings.range = get_i64_limited(1, INT32_MAX, "a: ");
		break;
	}
	save_config_file();
}
