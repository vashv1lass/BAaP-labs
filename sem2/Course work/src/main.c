/**
 * @file  main.c
 * @brief Contains the main code of an application.
 */

 #include <stdio.h>   // For `puts()`, `printf()`, etc.
 #include <stdbool.h> // For `true`.
 #include <string.h>  // For `strcpy()`, `strcmp()`.
 #include <stdlib.h>  // For `free()`, EXIT_SUCCESS.
 #include <errno.h>   // For `errno`.
 
 #include "safeio.h"      // For `safe_scanf()`, `instant_puts()`, etc.
 #include "binary_file.h" // For `binary_file_exists()`.
 #include "functional.h"  // For `create_file()`, `view_file()`, etc.
 #include "apartment.h"   // For `apartment`, `print_apartment()`, etc.
 #include "date.h"        // For `date`, `scan_date()`, etc.
 #include "database.h"    // For `get_apartment_by_id()`, `apartment_id_is_unused()`, etc.
 #include "logging.h"     // For `log_current_state()`, `log_binary_file_state()`, etc.
 
 int main(void) {
     // Initializing the log file.
     log_file_initialize();
     
     // Outputting welcoming message, logging the program start.
     printf(
         "Приложение: СИСТЕМА ПОДБОРА КВАРТИРЫ\n"
         "Разработано: Вашкевич Максим Викторович.\n"
         "Программа работает с бинарным файлом, в котором хранятся данные о квартирах. "
         "Файлов может быть несколько.\n"
         "Подробная информация об ошибках находится в файле \"%s\".\n",
         LOG_FILE_NAME
     );
     log_current_state("Работа программы начата. Было выведено приветственное сообщение.");
     
     while (true) {
         // Errno is reset before every action.
         errno = 0;
         
         // Writing the separator into the log file.
         log_current_state("------------------------------------------------------------------------------------------");
         
         // Giving the user a choice of options.
         instant_puts(
             "\nВведите строке ниже число, которое соответствует тому действию, которое вы хотите произвести:\n"
             "1 - Создать новый файл.\n"
             "2 - Просмотреть файл.\n"
             "3 - Добавить новую квартиру в файл.\n"
             "4 - Редактировать запись о квартире в файле.\n"
             "5 - Удалить квартиру из файла.\n"
             "6 - Поиск квартир по стоимости (линейный).\n"
             "7 - Поиск квартир по количеству комнат (бинарный).\n"
             "8 - Сортировка квартир по стоимости (быстрая сортировка).\n"
             "9 - Сортировка квартир по площади (сортировка выбором).\n"
             "10 - Сортировка квартир по дате добавления (сортировка вставками).\n"
             "11 - Поиск квартир по диапазону стоимости и количеству комнат.\n"
             "12 - Поиск свободных квартир, добавленных после определённой даты.\n"
             "13 - Просмотр файла логирования.\n"
             "Любое другое число - выход из программы.\n"
         );
         int option;
         // Input option.
         if (safe_scanf("%d", &option) != 1) {
             // If failed, outputting error message, logging error and terminating current iteration.
             printf(
                 "Ошибка ввода опции. Вероятно, введено некорректное значение.\n"
                 "Проверьте файл \"%s\" для подробностей.\n",
                 LOG_FILE_NAME
             );
             log_current_state("Меню: попытка ввода опции завершилась с ошибкой.");
             continue;
         }
         // Logging the users choice.
         log_current_state("Меню: была выбрана опция \"%d\".", option);
         
         if (!(1 <= option && option <= 13)) {
             // If the user chose the exit option, getting out from the cycle.
             break;
         }
         
         // If the option is viewing the log file.
         if (option == 13) {
             // Viewing the log file.
             view_log_file(stderr);
             
             continue;
         }
         
         // Input filename.
         instant_puts(
             "Введите в строке ниже имя файла, в котором (над которым) будет проводиться выбранное действие.\n"
         );
         char filename[FILENAME_MAX];
         if (safe_gets(filename, FILENAME_MAX) == NULL) {
             // If failed, outputting error message, logging error and terminating current iteration.
             printf(
                 "Ошибка ввода имени файла. Вероятно, введено некорректное значение.\n"
                 "Проверьте файл \"%s\" для подробностей.\n",
                 LOG_FILE_NAME
             );
             log_current_state("Меню: попытка ввода имени файла завершилась с ошибкой.\n\n");
             continue;
         }
         // Logging the users choice.
         log_current_state("Меню: выбранный файл для операции \"%s\".", filename);
         
         if (option != 1 && !binary_file_exists(filename)) {
             /*
              * If the file does not exist and the option wasn't the file creation, terminating current iteration.
              * Outputting the error message, logging error.
              */
             printf("Файл \"%s\" не найден в нужной директории.\n", filename);
             log_current_state("Меню: файл \"%s\" не был найден.", filename);
             continue;
         }
         
         switch (option) {
             case 1: {
                 // File creation.
                 
                 if (create_file(filename) == 0) {
                     // Success: logging it.
                     printf("\nФайл \"%s\" был успешно создан (пересоздан).\n", filename);
                     log_current_state("Создание файла: файл \"%s\" был создан (пересоздан).", filename);
                 } else {
                     // Failure: also logging it.
                     printf(
                         "\nФайл \"%s\" не был создан.\n"
                         "Скорее всего, пользователь не захотел пересоздавать существующий файл, "
                         "или произошла ошибка (маловероятно).\nДля подробностей смотреть файл \"%s\".\n",
                         filename,
                         LOG_FILE_NAME
                     );
                     log_current_state("Создание файла: файл \"%s\" не был создан.", filename);
                 }
                 
                 break;
             }
             case 2: {
                 // File viewing.
                 
                 instant_printf("\nФайл \"%s\" содержит следующую информацию:\n\n", filename);
                 if (view_file(stdout, filename) != -1) {
                     // Success: logging it.
                     log_current_state(
                         "Просмотр файла: содержимое файла \"%s\" было успешно выведено в консоль.",
                         filename
                     );
                 } else {
                     // Failure (file corrupted): logging it.
                     printf(
                         "ВНИМАНИЕ! Файл повреждён.\n"
                         "Информация выше (если она есть) может содержать некорректные данные.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Просмотр файла: попытка просмотра содержимого бинарного файла \"%s\" завершилась неудачей.",
                         filename
                     );
                 }
                 
                 break;
             }
             case 3: {
                 // Apartment addition to a file.
                 
                 // Input apartment.
                 apartment apt_to_add;
                 
                 apt_to_add.id = 0;
                 
                 // Input the address.
                 instant_puts("Введите адрес квартиры: ");
                 if (safe_gets(apt_to_add.address, ADDRESS_STRING_BUFFER_SIZE) == NULL) {
                     // If failure, outputting the error message, logging the error, terminating the operation.
                     printf(
                         "\nВозникли ошибки при вводе адреса квартиры.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Добавление квартиры в файл: возникла ошибка при чтении адреса квартиры.");
                     break;
                 }
                 // Logging the success of input.
                 log_current_state("Добавление квартиры в файл: адрес квартиры был успешно прочитан.");
                 
                 // Similarly.
                 instant_puts("Введите количество комнат в квартире: ");
                 if (safe_scanf("%d", &apt_to_add.rooms_count) != 1) {
                     printf(
                         "\nВозникли ошибки при вводе количества комнат в квартире.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Добавление квартиры в файл: чтение количества комнат в квартире завершилось с ошибкой."
                     );
                     break;
                 }
                 
                 if (apt_to_add.rooms_count <= 0) {
                     printf("\nОшибка. Количество комнат в квартире должно быть 1 или более.\n");
                     log_current_state(
                         "Добавление квартиры в файл: чтение количества комнат в квартире завершилось с ошибкой."
                     );
                     break;
                 }
                 
                 log_current_state("Добавление квартиры в файл: количество комнат в квартире было успешно прочитано.");
                 
                 // Similarly.
                 instant_puts("Введите площадь квартиры (в метрах квадратных): ");
                 if (safe_scanf("%f", &apt_to_add.area) != 1) {
                     printf(
                         "\nВозникли ошибки при вводе площади квартиры.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Добавление квартиры в файл: чтение площади квартиры завершилось с ошибкой.");
                     break;
                 }
                 
                 if (apt_to_add.area < AREA_EPSILON) {
                     printf("\nОшибка. Площадь квартиры должна быть не менее 0.1 м^2.\n");
                     log_current_state(
                         "Добавление квартиры в файл: чтение площади квартиры завершилось с ошибкой."
                     );
                     break;
                 }
                 
                 log_current_state("Добавление квартиры в файл: чтение площади квартиры завершено успешно.");
                 
                 // Similarly.
                 instant_puts("Введите этаж, на котором располагается квартира: ");
                 if (safe_scanf("%d", &apt_to_add.floor) != 1) {
                     printf(
                         "\nВозникли ошибки при вводе этажа, на котором располагается квартира.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Добавление квартиры в файл: чтение этажа, на котором располагается квартира, завершилось с "
                         "ошибкой."
                     );
                     break;
                 }
                 
                 if (apt_to_add.floor <= 0) {
                     printf("\nОшибка. Этаж квартиры должен быть 1 или более.\n");
                     log_current_state(
                         "Добавление квартиры в файл: чтение этажа квартиры завершилось с ошибкой."
                     );
                     break;
                 }
                 
                 log_current_state(
                     "Добавление квартиры в файл: чтение этажа, на котором располагается квартира завершилось успешно."
                 );
                 
                 // Similarly.
                 instant_puts("Введите стоимость квартиры (в долларах): ");
                 if (safe_scanf("%f", &apt_to_add.cost) != 1) {
                     printf(
                         "\nВозникли ошибки при вводе стоимости квартиры.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Добавление квартиры в файл: чтение стоимости квартиры завершилось с ошибкой.");
                     break;
                 }
                 
                 if (apt_to_add.cost < COST_EPSILON) {
                     printf("\nОшибка. Стоимость квартиры должна быть не менее $0.01.\n");
                     log_current_state(
                         "Добавление квартиры в файл: чтение стоимости квартиры завершилось с ошибкой."
                     );
                     break;
                 }
                 
                 log_current_state("Добавление квартиры в файл: чтение стоимости квартиры завершено успешно.");
                 
                 // Similarly.
                 instant_puts("Введите статус квартиры (0 - свободна, иное - продана): ");
                 if (safe_scanf("%d", &apt_to_add.sold) != 1) {
                     printf(
                         "\nВозникли ошибки при вводе статуса квартиры.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Добавление квартиры в файл: чтение статуса квартиры завершилось с ошибкой.");
                     break;
                 }
                 log_current_state("Добавление квартиры в файл: чтение статуса квартиры прошло успешно.");
                 
                 // Similarly.
                 instant_puts("Введите дату добавления квартиры в файл (формат ДД.ММ.ГГГГ): ");
                 if (scan_date(&apt_to_add.addition_date) != 1) {
                     printf(
                         "\nВозникли ошибки при вводе даты добавления квартиры.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Добавление квартиры в файл: чтение даты добавления квартиры в файл завершилось с ошибкой."
                     );
                     break;
                 }
                 log_current_state("Чтение даты добавления квартиры в файл прошло успешно.");
                 
                 // Checking the validity of entered apartment.
                 if (!apartment_is_valid(apt_to_add)) {
                     // If not valid, outputting the error message, logging the error and terminating current iteration.
                     puts("\nВведённые данные о квартире не являются корректными.");
                     log_current_state("Добавление квартиры в файл: введённые данные о квартире оказались некорректны!");
                     break;
                 }
                 
                 // Trying to add information to a file.
                 if (add_note_to_file(filename, &apt_to_add) == 0) {
                     // Addition success: logging it.
                     printf(
                         "\nДанные об этой квартире успешно добавлены в файл (Идентификатор квартиры: %d)\n",
                         apt_to_add.id
                     );
                     log_current_state("Добавление квартиры в файл: квартира была успешно добавлена в файл.");
                     log_apartment(apt_to_add, "Добавленная в файл квартира выглядит так:");
                     log_binary_file_state(filename, "Бинарный файл теперь выглядит следующим образом:");
                 } else {
                     // Addition failure: also logging it.
                     printf(
                         "\nВозникли проблемы при добавлении данных о квартире в файл!\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Добавление квартиры в файл: возникли ошибки при добавлении квартиры в файл.");
                 }
                 
                 break;
             }
             case 4: {
                 // Apartment editing in the file.
                 
                 // Entering the target apartment id.
                 instant_puts("Введите идентификатор квартиры, информацию о которой вы хотите изменить: ");
                 int apt_id_to_edit;
                 if (safe_scanf("%d", &apt_id_to_edit) != 1) {
                     // If something went wrong, logging error and terminating current operation.
                     printf(
                         "\nОшибка ввода. Вероятно, введено некорректное значение.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Изменение информации о квартире: "
                         "произошла ошибка ввода идентификатора изменяемой квартиры."
                     );
                     break;
                 }
                 // Logging the success of input.
                 log_current_state("Изменение информации о квартире: идентификатор изменяемой квартиры введён успешно.");
                 
                 // Getting apartment by ID.
                 apartment apt_to_edit = get_apartment_by_id(filename, apt_id_to_edit);
                 if (!apartment_is_valid(apt_to_edit)) {
                     // If apartment is invalid, there is no apartment with such id: logging it.
                     printf(
                         "\nКвартиры с таким идентификатором нет в файле, либо файл повреждён (маловероятно).\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Изменение информации о квартире: квартира с введённым идентификатором не найдена."
                     );
                     break;
                 }
                 
                 // Temporary variables for all apartment field types.
                 char tmp_string_buffer[STRING_BUFFER_MAX_SIZE];
                 int tmp_int;
                 float tmp_float;
                 
                 // Inputting new address.
                 instant_puts("Введите новый адрес квартиры (0 - оставить прежним): ");
                 if (safe_gets(tmp_string_buffer, STRING_BUFFER_MAX_SIZE) == NULL) {
                     // If something went wrong, logging error and terminating current operation.
                     printf(
                         "\nВозникли ошибки при вводе нового адреса квартиры.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Изменение информации о квартире: произошла ошибка при вводе нового адреса.");
                     break;
                 }
                 if (strcmp(tmp_string_buffer, "0") != 0) {
                     // If entered address value is not 0, updating it.
                     strncpy(apt_to_edit.address, tmp_string_buffer, ADDRESS_STRING_BUFFER_SIZE);
                 }
                 log_current_state("Изменение информации о квартире: новая информация об адресе была успешно введена.");
                 
                 // Similarly.
                 instant_puts("Введите новое количество комнат в квартире (0 - оставить прежним): ");
                 if (safe_scanf("%d", &tmp_int) != 1) {
                     printf(
                         "\nВозникли ошибки при вводе нового количества комнат в квартире.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Изменение информации о квартире: произошла ошибка при вводе нового количества комнат в "
                         "квартире."
                     );
                     break;
                 }
                 if (tmp_int != 0) {
                     if (tmp_int < 0) {
                         printf("\nОшибка. Количество комнат в квартире должно быть 1 или более.\n");
                         log_current_state(
                             "Изменение информации о квартире: чтение нового количества комнат в квартире "
                             "завершилось с ошибкой."
                         );
                         break;
                     }
                     apt_to_edit.rooms_count = tmp_int;
                 }
                 log_current_state(
                     "Изменение информации о квартире: новая информация о количестве комнат в квартире была успешно "
                     "введена."
                 );
                 
                 // Similarly.
                 instant_puts("Введите новую площадь квартиры (в метрах квадратных). 0 - оставить прежней: ");
                 if (safe_scanf("%f", &tmp_float) != 1) {
                     printf(
                         "\nВозникли ошибки при вводе новой площади квартиры.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Изменение информации о квартире: возникла ошибка при вводе новой информации о площади "
                         "квартиры."
                     );
                     break;
                 }
                 if (tmp_float != .0f) {
                     if (tmp_float < AREA_EPSILON) {
                         printf("\nОшибка. Площадь квартиры должна быть 0.1 м^2 или более.\n");
                         log_current_state(
                             "Изменение информации о квартире: чтение новой площади квартиры завершилось с ошибкой."
                         );
                         break;
                     }
                     apt_to_edit.area = tmp_float;
                 }
                 log_current_state(
                     "Изменение информации о квартире: новая информация о площади квартиры была успешно введена."
                 );
                 
                 // Similarly.
                 instant_puts("Введите новый этаж, на котором располагается квартира (0 - оставить прежним): ");
                 if (safe_scanf("%d", &tmp_int) != 1) {
                     printf(
                         "\nВозникли ошибки при вводе нового этажа, на котором располагается квартира.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Изменение информации о квартире: возникла ошибка при вводе новой информации об этаже, на "
                         "котором располагается квартира."
                     );
                     break;
                 }
                 if (tmp_int != 0) {
                     if (tmp_int < 0) {
                         printf("\nОшибка. Этаж квартиры должен быть 1 или более.\n");
                         log_current_state(
                             "Изменение информации о квартире: чтение этажа квартиры завершилось с ошибкой."
                         );
                         break;
                     }
                     apt_to_edit.floor = tmp_int;
                 }
                 log_current_state(
                     "Изменение информации о квартире: ввод новой информации об этаже, на котором располагается "
                     "квартира прошел успешно."
                 );
                 
                 // Similarly.
                 instant_puts("Введите новую стоимость квартиры (в долларах). 0 - оставить прежней: ");
                 if (safe_scanf("%f", &tmp_float) != 1) {
                     printf(
                         "\nВозникли ошибки при вводе новой стоимости квартиры.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Изменение информации о квартире: возникла ошибка при вводе новой информации о стоимости "
                         "квартиры."
                     );
                     break;
                 }
                 if (tmp_float != .0f) {
                     if (tmp_float < COST_EPSILON) {
                         printf("\nОшибка. Стоимость квартиры должна быть $0.01 или более.\n");
                         log_current_state(
                             "Изменение информации о квартире: чтение новой стоимости квартиры завершилось с ошибкой."
                         );
                         break;
                     }
                     apt_to_edit.cost = tmp_float;
                 }
                 log_current_state(
                     "Изменение информации о квартире: новая информация о стоимости квартиры была успешно "
                     "введена."
                 );
                 
                 // Similarly.
                 instant_puts("Введите новый статус квартиры (0 - оставить прежним, 1 - свободна, иное - продана): ");
                 if (safe_scanf("%d", &tmp_int) != 1) {
                     printf(
                         "\nВозникли ошибки при вводе нового статуса квартиры.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Изменение информации о квартире: возникла ошибка при вводе новой информации о статусе."
                     );
                     break;
                 }
                 if (tmp_int != 0) {
                     apt_to_edit.sold = tmp_int != 1;
                 }
                 log_current_state("Изменение информации о квартире: новая информация о статусе была успешно введена.");
                 
                 // Similarly.
                 instant_puts(
                     "Введите новую дату добавления квартиры в файл (формат ДД.ММ.ГГГГ). 0 - оставить прежней: "
                 );
                 if (safe_gets(tmp_string_buffer, STRING_BUFFER_MAX_SIZE) == NULL) {
                     printf(
                         "\nВозникли ошибки при вводе новой даты добавления квартиры.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Изменение информации о квартире: возникла ошибка при вводе новой информации о дате добавления "
                         "квартиры в файл."
                     );
                     break;
                 }
                 if (strcmp(tmp_string_buffer, "0") != 0) {
                     apt_to_edit.addition_date = str_to_date(tmp_string_buffer);
                     // Checking if the entered date is correct.
                     if (!date_is_valid(apt_to_edit.addition_date)) {
                         // If not, logging the error and terminating the current operation.
                         printf(
                             "\nВозникли ошибки при вводе новой даты добавления квартиры.\n"
                             "Подробнее: см. файл \"%s\".\n",
                             LOG_FILE_NAME
                         );
                         log_current_state(
                             "Изменение информации о квартире: возникла ошибка при вводе новой информации о дате "
                             "добавления квартиры в файл."
                         );
                         break;
                     }
                 }
                 log_current_state(
                     "Изменение информации о квартире: новая информация о дате добавления квартиры в файл была успешно "
                     "введена."
                 );
                 
                 // Checking the validity of entered apartment.
                 if (!apartment_is_valid(apt_to_edit)) {
                     // If invalid, terminating the current operation.
                     printf(
                         "\nВведённые данные о квартире не являются корректными.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Изменение информации о квартире: новая информация о квартире не является корректной."
                     );
                     break;
                 }
                 
                 // Trying to edit the note.
                 if (edit_note_in_file(filename, apt_id_to_edit, apt_to_edit) == 0) {
                     // Success: logging it.
                     puts("\nДанные об этой квартире успешно отредактированы.");
                     log_current_state(
                         "Изменение информации о квартире: информация о квартире была успешно отредактирована."
                     );
                     log_apartment(apt_to_edit, "Новая информация о квартире:");
                     log_binary_file_state(filename, "Файл теперь выглядит следующим образом:");
                 } else {
                     // Failure: also logging it.
                     printf(
                         "\nПроизошла ошибка при редактировании информации в файле.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Изменение информации о квартире: возникла ошибка при редактировании информации о квартире"
                     );
                 }
                 
                 break;
             }
             case 5: {
                 // Apartment deletion from the file.
                 
                 // Getting the target apartment id.
                 instant_puts("Введите идентификатор квартиры, информацию о которой вы хотите удалить: ");
                 int apt_id_to_remove;
                 if (safe_scanf("%d", &apt_id_to_remove) != 1) {
                     // If something went wrong, logging errors and terminating the current operation.
                     printf(
                         "\nОшибка ввода. Вероятно, введено некорректное значение.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Удаление информации о квартире из файла: "
                         "произошла ошибка ввода идентификатора удаляемой квартиры."
                     );
                     break;
                 }
                 // Logging the success of input.
                 log_current_state(
                     "Удаление информации о квартире из файла: идентификатор удаляемой квартиры введён успешно."
                 );
                 
                 // Checking if apartment with such id is in the file.
                 apartment apt_to_remove = get_apartment_by_id(filename, apt_id_to_remove);
                 if (!apartment_is_valid(apt_to_remove)) {
                     // If not, logging it and terminating the current operation.
                     puts("\nКвартиры с таким идентификатором нет в файле.");
                     log_current_state(
                         "Удаление информации о квартире из файла: квартира с введённым идентификатором не найдена."
                     );
                     break;
                 }
                 
                 // Trying to remove the note.
                 if (remove_note_from_file(filename, apt_id_to_remove) == 0) {
                     // Success: logging it.
                     puts("\nИнформация о квартире успешно удалена из файла.");
                     log_current_state("Удаление информации о квартире: квартира была удалена из файла успешно");
                     log_apartment(apt_to_remove, "Удалённая квартира: ");
                     log_binary_file_state(filename, "Файл теперь выглядит следующим образом: ");
                 } else {
                     // Failure: also logging it.
                     printf(
                         "\nПроизошла ошибка при удалении информации из файла. Возможно, файл повреждён.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Удаление информации о квартире: возникла ошибка при удалении квартиры из файла."
                     );
                 }
                 
                 break;
             }
             case 6: {
                 // Linear search by cost.
                 
                 // Entering the cost to find in file.
                 instant_puts("Введите стоимость квартир, информацию о которых вы хотите найти: ");
                 float apartment_cost_to_find;
                 if (safe_scanf("%f", &apartment_cost_to_find) != 1 || apartment_cost_to_find < COST_EPSILON) {
                     // If something went wrong, logging the error and terminating the current operation.
                     printf(
                         "\nОшибка ввода. Вероятно, введено некорректное значение.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Линейный поиск по стоимости квартиры: ввод стоимости завершился с ошибкой.");
                     break;
                 }
                 // Logging the success of input.
                 log_current_state("Линейный поиск по стоимости квартиры: ввод стоимости завершился успешно.");
                 
                 // Trying to find fitting apartments.
                 size_t found_apartments_by_cost_count;
                 apartment *found_apartments_by_cost = linear_search_by_cost(
                     filename,
                     apartment_cost_to_find,
                     &found_apartments_by_cost_count
                 );
                 
                 if (found_apartments_by_cost_count == (size_t)-1) {
                     // Error: logging it.
                     printf(
                         "\nПроизошла ошибка при поиске квартир.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Линейный поиск по стоимости квартиры: поиск завершился с ошибкой.");
                 } else if (found_apartments_by_cost_count == 0) {
                     // No such apartments found: logging it.
                     printf("\nКвартир со стоимостью $%.2f не найдено.\n", apartment_cost_to_find);
                     log_current_state(
                         "Линейный поиск по стоимости квартиры: квартиры с введённой стоимостью ($%.2f) не "
                         "были найдены.",
                         apartment_cost_to_find
                     );
                 } else {
                     // Apartments found. Outputting them.
                     printf("\nКвартиры со стоимостью $%.2f:\n\n", apartment_cost_to_find);
                     for (size_t i = 0; i < found_apartments_by_cost_count; i++) {
                         if (print_apartment(found_apartments_by_cost[i]) == -1) {
                             // Output failure: logging the error.
                             printf(
                                 "Произошла ошибка при выводе одной из квартир.\n"
                                 "Подробнее: см. файл \"%s\".\n",
                                 LOG_FILE_NAME
                             );
                             log_current_state(
                                 "Линейный поиск по стоимости квартиры: квартира, идущая в файле на месте "
                                 "номер %zu не была выведена (возникла ошибка).",
                                 i + 1
                             );
                         }
                         
                         puts("");
                         
                         if (i < found_apartments_by_cost_count - 1) {
                             // Newline for the apartments that are not last in file.
                             puts("");
                         }
                     }
                     
                     // Freeing the found apartments.
                     free(found_apartments_by_cost);
                     
                     // Logging the success.
                     log_current_state(
                         "Линейный поиск по стоимости квартиры: квартиры с введённой стоимостью ($%.2f) "
                         "были выведены в консоль (только квартиры, с которыми не возникло проблем при выводе).",
                         apartment_cost_to_find
                     );
                 }
                 
                 break;
             }
             case 7: {
                 // Binary search by rooms count.
                 
                 // Getting the rooms count.
                 instant_puts("Введите количество комнат в квартирах, информацию о которых вы хотите найти: ");
                 int apartment_rooms_count_to_find;
                 if (safe_scanf("%d", &apartment_rooms_count_to_find) != 1 || apartment_rooms_count_to_find < 1) {
                     // If something went wrong, logging the error and terminating the current operation.
                     printf(
                         "\nОшибка ввода. Вероятно, введено некорректное значение.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Бинарный поиск по количеству комнат в квартире: ввод количества комнат завершился с ошибкой."
                     );
                     break;
                 }
                 // Logging the input success.
                 log_current_state(
                     "Бинарный поиск по количеству комнат в квартире: ввод количества комнат завершился успешно."
                 );
                 
                 // Trying to find fitting apartments.
                 size_t found_apartments_by_rooms_count_count;
                 apartment *found_apartments_by_rooms_count = binary_search_by_rooms_count(
                     filename,
                     apartment_rooms_count_to_find,
                     &found_apartments_by_rooms_count_count
                 );
                 
                 if (found_apartments_by_rooms_count_count == (size_t)-1) {
                     // Error: logging it.
                     printf(
                         "\nПроизошла ошибка при поиске квартир.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Бинарный поиск по количеству комнат в квартире: поиск завершился с ошибкой.");
                 } else if (found_apartments_by_rooms_count_count == 0) {
                     // No such apartments found: logging it.
                     printf("\nКвартир с количеством комнат %d не найдено.\n", apartment_rooms_count_to_find);
                     log_current_state(
                         "Бинарный поиск по количеству комнат в квартире: квартир с количеством комнат "
                         "%d не найдено.",
                         apartment_rooms_count_to_find
                     );
                 } else {
                     // Success.
                     printf("\nНайденные квартиры с количеством комнат %d:\n\n", apartment_rooms_count_to_find);
                     for (size_t i = 0; i < found_apartments_by_rooms_count_count; i++) {
                         // Apartments found. Outputting them.
                         if (print_apartment(found_apartments_by_rooms_count[i]) == -1) {
                             // Output failure: logging the error.
                             printf(
                                 "Произошла ошибка при выводе одной из квартир."
                                 "Подробнее: см. файл \"%s\".\n",
                                 LOG_FILE_NAME
                             );
                             log_current_state(
                                 "Бинарный поиск по количеству комнат в квартире: квартира, идущая в файле на месте "
                                 "номер %zu не была выведена (возникла ошибка).",
                                 i + 1
                             );
                         }
                         
                         puts("");
                         
                         if (i < found_apartments_by_rooms_count_count - 1) {
                             // Newline for the apartments that are not last in file.
                             puts("");
                         }
                     }
                     
                     // Freeing the found apartments.
                     free(found_apartments_by_rooms_count);
                     
                     // Logging the success.
                     log_current_state(
                         "Бинарный поиск по количеству комнат в квартире: квартиры с введённым количеством комнат (%d) "
                         "были выведены в консоль (квартиры, с которыми не возникло проблем при выводе).",
                         apartment_rooms_count_to_find
                     );
                 }
                 
                 break;
             }
             case 8: {
                 // Sorting apartments by cost (quicksort).
                 
                 if (quicksort_by_cost(filename) == 0) {
                     // Success: logging it.
                     printf("\nФайл \"%s\" успешно отсортирован по неубыванию стоимости.\n", filename);
                     log_current_state("Быстрая cортировка квартир по стоимости: сортировка выполнена успешно.");
                     log_binary_file_state(filename, "Упорядоченные по неубыванию стоимости квартиры: ");
                 } else {
                     // Failure: also logging it.
                     printf(
                         "\nВозникла ошибка при сортировке файла.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Быстрая cортировка квартир по стоимости: возникла ошибка при сортировке.");
                 }
                 
                 break;
             }
             case 9: {
                 // Sorting apartments by area (selection sort).
                 
                 if (selection_sort_by_area(filename) == 0) {
                     // Success: logging it.
                     printf("\nФайл \"%s\" успешно отсортирован по неубыванию площади квартир.\n", filename);
                     log_current_state("Сортировка квартир выбором по площади: сортировка выполнена успешно.");
                     log_binary_file_state(filename, "Упорядоченные по неубыванию площади квартиры: ");
                 } else {
                     // Failure: also logging it.
                     printf(
                         "\nВозникла ошибка при сортировке файла.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Cортировка квартир выбором по площади: возникла ошибка при сортировке.");
                 }
                 
                 break;
             }
             case 10: {
                 // Sorting apartments by addition date (insertion sort).
                 
                 if (insertion_sort_by_addition_date(filename) == 0) {
                     // Success: logging it.
                     printf("\nФайл \"%s\" успешно отсортирован по неубыванию даты добавления.\n", filename);
                     log_current_state("Сортировка квартир вставками по дате добавления: сортировка выполнена успешно.");
                     log_binary_file_state(filename, "Упорядоченные по неубыванию дат добавления квартиры: ");
                 } else {
                     // Failure: also logging it.
                     printf(
                         "\nВозникла ошибка при сортировке файла.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state("Cортировка квартир вставками по дате добавления: возникла ошибка.");
                 }
                 
                 break;
             }
             case 11: {
                 // Searching apartments by cost range and rooms count.
                 
                 // Getting the low price border.
                 instant_puts("Введите нижнюю границу стоимости квартир, информацию о которых вы хотите найти: ");
                 float apartment_cost_to_find_low;
                 if (safe_scanf("%f", &apartment_cost_to_find_low) != 1 || apartment_cost_to_find_low < COST_EPSILON) {
                     // If something went wrong, logging the error and terminating the current operation.
                     printf(
                         "\nОшибка ввода. Вероятно, введено некорректное значение.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Поиск по признаку (диапазон стоимости и количество комнат): "
                         "ввод нижней границы стоимости завершился с ошибкой."
                     );
                     break;
                 }
                 // Logging the success of input.
                 log_current_state(
                     "Поиск по признаку (диапазон стоимости и количество комнат): "
                     "ввод нижней границы стоимости завершился успешно."
                 );
                 
                 // Similarly.
                 instant_puts("Введите верхнюю границу стоимости квартир, информацию о которых вы хотите найти: ");
                 float apartment_cost_to_find_high;
                 if (safe_scanf("%f", &apartment_cost_to_find_high) != 1 || apartment_cost_to_find_high < COST_EPSILON) {
                     printf(
                         "\nОшибка ввода. Вероятно, введено некорректное значение.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Поиск по признаку (диапазон стоимости и количество комнат): "
                         "ввод верхней границы стоимости завершился с ошибкой."
                     );
                     break;
                 }
                 log_current_state(
                     "Поиск по признаку (диапазон стоимости и количество комнат): "
                     "ввод верхней границы стоимости завершился успешно."
                 );
                 
                 if (apartment_cost_to_find_low > apartment_cost_to_find_high) {
                     printf("\nОшибка. Неверный диапазон.\n");
                     log_current_state(
                         "Поиск по признаку (диапазон стоимости и количество комнат): "
                         "ввод диапазона стоимости завершился с ошибкой (верхняя граница стоимости меньше нижней)."
                     );
                     break;
                 }
                 
                 // Similarly.
                 instant_puts("Введите количество комнат в квартирах, информацию о которых вы хотите найти: ");
                 int apartment_rooms_count_to_find;
                 if (safe_scanf("%d", &apartment_rooms_count_to_find) != 1 || apartment_rooms_count_to_find < 1) {
                     printf(
                         "\nОшибка ввода. Вероятно, введено некорректное значение.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Поиск по признаку (диапазон стоимости и количество комнат): "
                         "ввод количества комнат завершился с ошибкой."
                     );
                     break;
                 }
                 log_current_state(
                     "Поиск по признаку (диапазон стоимости и количество комнат): "
                     "ввод количества комнат завершился с успешно."
                 );
                 
                 // Trying to find fitting apartments.
                 size_t found_apartments_by_criteria_count;
                 apartment *found_apartments_by_criteria = search_by_cost_range_and_rooms_count(
                     filename,
                     apartment_cost_to_find_low,
                     apartment_cost_to_find_high,
                     apartment_rooms_count_to_find,
                     &found_apartments_by_criteria_count
                 );
                 
                 if (found_apartments_by_criteria_count == (size_t)-1) {
                     // Error: logging it.
                     printf(
                         "\nПроизошла ошибка при поиске квартир.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Поиск по признаку (диапазон стоимости и количество комнат): "
                         "поиск завершился с ошибкой."
                     );
                 } else if (found_apartments_by_criteria_count == 0) {
                     // No such apartments found: logging it.
                     printf("\nКвартир с такими параметрами в файле %s не найдено.\n", filename);
                     log_current_state(
                         "Поиск по признаку (диапазон стоимости и количество комнат): "
                         "квартир с введёнными параметрами не найдено."
                     );
                 } else {
                     // Apartments found. Outputting them.
                     puts("\nКвартиры, удовлетворяющие введённым параметрам:\n");
                     for (size_t i = 0; i < found_apartments_by_criteria_count; i++) {
                         if (print_apartment(found_apartments_by_criteria[i]) == -1) {
                             // Output failure: logging the error.
                             printf(
                                 "Произошла ошибка при выводе одной из квартир.\n"
                                 "Подробнее: см. файл \"%s\".\n",
                                 LOG_FILE_NAME
                             );
                             log_current_state(
                                 "Поиск по признаку (диапазон стоимости и количество комнат): квартира, идущая в файле "
                                 "на месте номер %zu не была выведена (возникла ошибка).",
                                 i + 1
                             );
                         }
                         
                         puts("");
                         
                         if (i < found_apartments_by_criteria_count - 1) {
                             // Newline for the apartments that are not last in file.
                             puts("");
                         }
                     }
                     
                     // Freeing the array of found apartments.
                     free(found_apartments_by_criteria);
                     
                     // Logging the success.
                     log_current_state(
                         "Поиск по признаку (диапазон стоимости и количество комнат): квартиры с введёнными параметрами "
                         "были выведены в консоль."
                     );
                 }
                 
                 break;
             }
             case 12: {
                 // Searching newest free apartments.
                 
                 // Entering the earliest date of addition.
                 instant_puts("Введите дату, после которой должны были быть добавлены искомые квартиры: ");
                 date apartment_addition_date_to_find;
                 if (scan_date(&apartment_addition_date_to_find) != 1) {
                     // If failed, logging the error, terminating the current operation.
                     printf(
                         "\nОшибка ввода. Вероятно, введено некорректное значение.\n"
                         "Подробнее: см. файл \"%s\".\n",
                         LOG_FILE_NAME
                     );
                     log_current_state(
                         "Статистика (добавленные после определённой даты свободные квартиры): "
                         "Произошла ошибка при вводе даты добавления."
                     );
                     break;
                 }
                 // Logging the input success.
                 log_current_state(
                     "Статистика (добавленные после определённой даты свободные квартиры): "
                     "Ввод даты добавления успешно выполнен."
                 );
                 
                 // Trying to find fitting apartments.
                 size_t found_apartments_by_addition_date_count;
                 apartment *found_apartments_by_addition_date = search_newest_free_apartments(
                     filename,
                     apartment_addition_date_to_find,
                     &found_apartments_by_addition_date_count
                 );
                 
                 if (found_apartments_by_addition_date_count == (size_t)-1) {
                     // Error: logging it.
                     puts("\nПроизошла ошибка при поиске квартир.");
                     log_current_state(
                         "Статистика (добавленные после определённой даты свободные квартиры): "
                         "поиск завершился с ошибкой."
                     );
                 } else if (found_apartments_by_addition_date_count == 0) {
                     // No such apartments found: logging it.
                     puts("\nСвободных квартир, добавленных после введённой даты, не найдено.");
                     log_current_state(
                         "Статистика (добавленные после определённой даты свободные квартиры): "
                         "таких квартир не найдено."
                     );
                 } else {
                     // Apartments found. Outputting them.
                     puts("\nНайденные свободные квартиры, появившиеся в базе после определённой даты:\n");
                     for (size_t i = 0; i < found_apartments_by_addition_date_count; i++) {
                         if (print_apartment(found_apartments_by_addition_date[i]) == -1) {
                             // Output failure: logging the error.
                             printf(
                                 "Произошла ошибка при выводе одной из квартир.\n"
                                 "Подробнее: см. файл \"%s\".\n",
                                 LOG_FILE_NAME
                             );
                             log_current_state(
                                 "Статистика (добавленные после определённой даты свободные квартиры): квартира, идущая "
                                 "в файле на месте номер %zu не была выведена (возникла ошибка).",
                                 i + 1
                             );
                         }
                         
                         puts("");
                         
                         if (i < found_apartments_by_addition_date_count - 1) {
                             // Newline for the apartments that are not last in file.
                             puts("");
                         }
                     }
                     
                     // Freeing the array of found apartments.
                     free(found_apartments_by_addition_date);
                     
                     // Logging the success.
                     log_current_state(
                         "Статистика (добавленные после определённой даты свободные квартиры): квартиры с введёнными "
                         "параметрами были выведены в консоль."
                     );
                 }
                 
                 break;
             }
             default: {
                 break;
             }
         }
     }
     
     // Outputting the farewell message, logging the finish.
     instant_puts("\nРабота программы завершена.\n");
     log_current_state("Работа программы была завершена.");
     
     return EXIT_SUCCESS;
 }