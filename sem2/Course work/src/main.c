/**
 * @file  main.c
 * @brief Contains the main code of an application.
 */

 #include <stdio.h>   // For `fputs()`, `printf()`, etc.
 #include <stdbool.h> // For `true`.
 #include <string.h>  // For `strcspn()`.
 #include <stdlib.h>  // For `free()`.
 
 #include "safeio.h"      // For `safe_fscanf()`, `instant_fputs()`, etc.
 #include "binary_file.h" // For `binary_file_exists()`.
 #include "functional.h"  // For `create_file()`, `view_file()`, etc.
 #include "apartment.h"   // For `apartment`, `fprint_apartment()`, etc.
 #include "date.h"        // For `date`, `fscan_date()`, etc.
 #include "database.h"    // For `get_apartment_by_id()`, `apartment_id_is_unused()`, etc.
 
 int main(void) {
     // Welcoming message.
     fputs("Приложение: СИСТЕМА ПОДБОРА КВАРТИРЫ\n"
           "Разработано: Вашкевич Максим Викторович.\n"
           "Программа работает с бинарным файлом, в котором хранятся данные о квартирах. "
           "Файлов может быть несколько.\n", stdout);
     
     while (true) {
         // Giving the user a choice of options.
         instant_fputs("\nВведите строке ниже число, которое соответствует тому действию, которое вы хотите произвести:\n"
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
                       "Любое другое число - выход из программы.\n", stdout);
         int option;
         // Input option.
         if (safe_fscanf(stdin, "%d", &option) != 1) {
             // If failed, terminating current iteration.
             fputs("Ошибка ввода. Вероятно, введено некорректное значение.\n", stdout);
             continue;
         }
         
         if (!(1 <= option && option <= 12)) {
             // If the user chose the exit option, getting out from the cycle.
             break;
         }
         
         // Input filename.
         instant_fputs("Введите в строке ниже имя файла, в котором (над которым) будет проводиться выбранное действие."
                       "\n", stdout);
         char filename[FILENAME_MAX];
         if (safe_fgets(filename, FILENAME_MAX, stdin) == NULL) {
             // If failed, terminating current iteration.
             fputs("Что-то пошло не так при вводе имени файла.\n", stdout);
             continue;
         }
         filename[strcspn(filename, "\n")] = '\0';
         
         if (option != 1 && !binary_file_exists(filename)) {
             // If the file does not exist and the option wasn't the file creation, terminating current iteration.
             printf("Файл \"%s\" не найден.\n", filename);
             continue;
         }
         
         switch (option) {
             case 1: {
                 // File creation.
                 
                 if (create_file(filename) == 0) {
                     // Success.
                     printf("Файл \"%s\" был успешно создан (пересоздан).\n", filename);
                 } else {
                     // Failure.
                     printf("Файл \"%s\" не был создан. Скорее всего, пользователь не захотел пересоздавать "
                            "существующий файл, или произошла ошибка (маловероятно)\n", filename);
                 }
                 
                 break;
             }
             case 2: {
                 // File viewing.
                 
                 instant_fprintf(stdout, "Файл \"%s\" содержит следующую информацию:\n\n", filename);
                 if (view_file(stdout, filename) == -1) {
                     // Failure (file corrupted).
                     fputs("ВНИМАНИЕ! Файл повреждён.\n"
                           "Информация выше (если она есть) может содержать некорректные данные.\n", stdout);
                 }
                 
                 break;
             }
             case 3: {
                 // Apartment addition to a file.
                 
                 // Input apartment.
                 apartment apt_to_add;
                 
                 apt_to_add.id = 0;
                 
                 // Input the address.
                 instant_fputs("Введите адрес квартиры: ", stdout);
                 if (safe_fgets(apt_to_add.address, STRING_BUFFER_MAX_SIZE, stdin) == NULL) {
                     // If failure, terminating it.
                     fputs("Возникли ошибки при вводе адреса квартиры.\n", stdout);
                     break;
                 }
                 
                 // -//-
                 instant_fputs("Введите количество комнат в квартире: ", stdout);
                 if (safe_fscanf(stdin, "%d", &apt_to_add.rooms_count) != 1) {
                     fputs("Возникли ошибки при вводе количества комнат в квартире.\n", stdout);
                     break;
                 }
                 
                 // -//-
                 instant_fputs("Введите площадь квартиры (в метрах квадратных): ", stdout);
                 if (safe_fscanf(stdin, "%f", &apt_to_add.area) != 1) {
                     fputs("Возникли ошибки при вводе площади квартиры.\n", stdout);
                     break;
                 }
                 
                 // -//-
                 instant_fputs("Введите этаж, на котором располагается квартира: ", stdout);
                 if (safe_fscanf(stdin, "%d", &apt_to_add.floor) != 1) {
                     fputs("Возникли ошибки при вводе этажа, на котором располагается квартира.\n", stdout);
                     break;
                 }
                 
                 // -//-
                 instant_fputs("Введите стоимость квартиры (в долларах): ", stdout);
                 if (safe_fscanf(stdin, "%f", &apt_to_add.cost) != 1) {
                     fputs("Возникли ошибки при вводе стоимости квартиры.\n", stdout);
                     break;
                 }
                 
                 // -//-
                 instant_fputs("Введите статус квартиры (0 - свободна, иное - продана): ", stdout);
                 if (safe_fscanf(stdin, "%d", &apt_to_add.sold) != 1) {
                     fputs("Возникли ошибки при вводе статуса квартиры.\n", stdout);
                     break;
                 }
                 
                 // -//-
                 instant_fputs("Введите дату добавления квартиры в файл (формат ДД.ММ.ГГГГ): ", stdout);
                 if (fscan_date(stdin, &apt_to_add.addition_date) != 1) {
                     fputs("Возникли ошибки при вводе даты добавления квартиры.\n", stdout);
                     break;
                 }
                 
                 // Checking the validity of entered apartment.
                 if (!apartment_is_valid(apt_to_add)) {
                     // If not valid, terminating current iteration.
                     fputs("Введённые данные о квартире не являются корректными.\n", stdout);
                     break;
                 }
                 
                 // Trying to add information to a file.
                 if (add_note_to_file(filename, &apt_to_add) == 0) {
                     // Addition success.
                     fprintf(stdout,
                             "Данные об этой квартире успешно добавлены в файл (Идентификатор квартиры: %d)\n",
                             apt_to_add.id);
                 } else {
                     // Addition failure.
                     fputs("Возникли проблемы при добавлении данных о квартире в файл!\n", stdout);
                 }
                 
                 break;
             }
             case 4: {
                 // Apartment editing in the file.
                 
                 // Entering the target apartment id.
                 instant_fputs("Введите идентификатор квартиры, информацию о которой вы хотите изменить: ", stdout);
                 int apt_id_to_edit;
                 if (safe_fscanf(stdin, "%d", &apt_id_to_edit) != 1) {
                     // If something went wrong, terminating current iteration.
                     fputs("Ошибка ввода. Вероятно, введено некорректное значение.\n", stdout);
                     break;
                 }
                 
                 // Getting apartment by ID.
                 apartment apt_to_edit = get_apartment_by_id(filename, apt_id_to_edit);
                 if (!apartment_is_valid(apt_to_edit)) {
                     // If apartment is invalid, there is no apartment with such id.
                     fputs("Квартиры с таким идентификатором нет в файле, либо файл повреждён (маловероятно).\n",
                           stdout);
                     break;
                 }
                 
                 // Temporary variables for all apartment field types.
                 char tmp_string_buffer[STRING_BUFFER_MAX_SIZE];
                 int tmp_int;
                 float tmp_float;
                 
                 // Entering new address.
                 instant_fputs("Введите новый адрес квартиры (0 - оставить прежним): ", stdout);
                 if (safe_fgets(tmp_string_buffer, STRING_BUFFER_MAX_SIZE, stdin) == NULL) {
                     // If something went wrong, terminating current iteration.
                     fputs("Возникли ошибки при вводе нового адреса квартиры.\n", stdout);
                     break;
                 }
                 if (strcmp(tmp_string_buffer, "0") != 0) {
                     // If entered address value is not 0, updating it.
                     strncpy(apt_to_edit.address, tmp_string_buffer, STRING_BUFFER_MAX_SIZE);
                 }
                 
                 // Similarly.
                 instant_fputs("Введите новое количество комнат в квартире (0 - оставить прежним): ", stdout);
                 if (safe_fscanf(stdin, "%d", &tmp_int) != 1) {
                     fputs("Возникли ошибки при вводе нового количества комнат в квартире.\n", stdout);
                     break;
                 }
                 if (tmp_int != 0) {
                     apt_to_edit.rooms_count = tmp_int;
                 }
                 
                 // Similarly.
                 instant_fputs("Введите новую площадь квартиры (в метрах квадратных). 0 - оставить прежней: ", stdout);
                 if (safe_fscanf(stdin, "%f", &tmp_float) != 1) {
                     fputs("Возникли ошибки при вводе новой площади квартиры.\n", stdout);
                     break;
                 }
                 if (tmp_float != .0f) {
                     apt_to_edit.area = tmp_float;
                 }
                 
                 // Similarly.
                 instant_fputs("Введите новый этаж, на котором располагается квартира (0 - оставить прежним): ", stdout);
                 if (safe_fscanf(stdin, "%d", &tmp_int) != 1) {
                     fputs("Возникли ошибки при вводе нового этажа, на котором располагается квартира.\n", stdout);
                     break;
                 }
                 if (tmp_int != 0) {
                     apt_to_edit.floor = tmp_int;
                 }
                 
                 // Similarly.
                 instant_fputs("Введите новую стоимость квартиры (в долларах). 0 - оставить прежней: ", stdout);
                 if (safe_fscanf(stdin, "%f", &tmp_float) != 1) {
                     fputs("Возникли ошибки при вводе новой стоимости квартиры.\n", stdout);
                     break;
                 }
                 if (tmp_float != .0f) {
                     apt_to_edit.cost = tmp_float;
                 }
                 
                 // Similarly.
                 instant_fputs("Введите новый статус квартиры (0 - оставить прежним, 1 - свободна, иное - продана): ",
                               stdout);
                 if (safe_fscanf(stdin, "%d", &tmp_int) != 1) {
                     fputs("Возникли ошибки при вводе нового статуса квартиры.\n", stdout);
                     break;
                 }
                 if (tmp_int != 0) {
                     apt_to_edit.sold = tmp_int != 1;
                 }
                 
                 // Similarly.
                 instant_fputs("Введите новую дату добавления квартиры в файл (формат ДД.ММ.ГГГГ). "
                               "0 - оставить прежней: ", stdout);
                 if (safe_fgets(tmp_string_buffer, STRING_BUFFER_MAX_SIZE, stdin) == NULL) {
                     fputs("Возникли ошибки при вводе новой даты добавления квартиры.\n", stdout);
                     break;
                 }
                 if (strcmp(tmp_string_buffer, "0") != 0) {
                     apt_to_edit.addition_date = str_to_date(tmp_string_buffer);
                 }
                 
                 // Checking the validity of entered apartment.
                 if (!apartment_is_valid(apt_to_edit)) {
                     // If invalid, terminating the current iteration.
                     fputs("Введённые данные о квартире не являются корректными.\n", stdout);
                     break;
                 }
                 
                 // Trying to edit the note.
                 if (edit_note_in_file(filename, apt_id_to_edit, apt_to_edit) == 0) {
                     // Success.
                     fputs("Данные об этой квартире успешно отредактированы.\n", stdout);
                 } else {
                     // Failure.
                     fputs("Произошла ошибка при редактировании информации в файле.\n", stdout);
                 }
                 
                 break;
             }
             case 5: {
                 // Apartment deletion from the file.
                 
                 // Getting the target apartment id.
                 instant_fputs("Введите идентификатор квартиры, информацию о которой вы хотите удалить: ", stdout);
                 int apartment_id_to_remove;
                 if (safe_fscanf(stdin, "%d", &apartment_id_to_remove) != 1) {
                     // If something went wrong, terminating the current iteration.
                     fputs("Ошибка ввода. Вероятно, введено некорректное значение.\n", stdout);
                     break;
                 }
                 
                 // Checking if apartment with such id is in the file.
                 if (apartment_id_is_unused(filename, apartment_id_to_remove)) {
                     // If not, terminating the current iteration.
                     fputs("Квартиры с таким идентификатором нет в файле.\n", stdout);
                     break;
                 }
                 
                 // Trying to remove the note.
                 if (remove_note_from_file(filename, apartment_id_to_remove) == 0) {
                     // Success.
                     fputs("Информация о квартире успешно удалена из файла.\n", stdout);
                 } else {
                     // Failure.
                     fputs("Произошла ошибка при удалении информации из файла. Возможно, файл повреждён.\n", stdout);
                 }
                 
                 break;
             }
             case 6: {
                 // Linear search by cost.
                 
                 // Entering the cost to find in file.
                 instant_fputs("Введите стоимость квартир, информацию о которых вы хотите найти: ", stdout);
                 float apartment_cost_to_find;
                 if (safe_fscanf(stdin, "%f", &apartment_cost_to_find) != 1) {
                     // If something went wrong, terminating the current iteration.
                     fputs("Ошибка ввода. Вероятно, введено некорректное значение.\n", stdout);
                     break;
                 }
                 
                 // Trying to find fitting apartments.
                 size_t found_apartments_by_cost_count;
                 apartment *found_apartments_by_cost = linear_search_by_cost(
                     filename,
                     apartment_cost_to_find,
                     &found_apartments_by_cost_count
                 );
                 
                 if (found_apartments_by_cost_count == (size_t)-1) {
                     // Error.
                     fputs("Произошла ошибка при поиске квартир.\n", stdout);
                 } else if (found_apartments_by_cost_count == 0) {
                     // No such apartments found.
                     printf("Квартир со стоимостью $%.2f не найдено.\n", apartment_cost_to_find);
                 } else {
                     // Apartments found. Outputting them.
                     printf("Квартиры со стоимостью $%.2f:\n\n", apartment_cost_to_find);
                     for (size_t i = 0; i < found_apartments_by_cost_count; i++) {
                         if (fprint_apartment(stdout, found_apartments_by_cost[i]) == -1) {
                             // Output failure.
                             fputs("Произошла ошибка при выводе одной из квартир.\n", stdout);
                         }
                         
                         fputs("\n", stdout);
                         
                         if (i < found_apartments_by_cost_count - 1) {
                             // Newline for the apartments that are not last in file.
                             fputs("\n", stdout);
                         }
                     }
                     
                     // Freeing the found apartments.
                     free(found_apartments_by_cost);
                 }
                 
                 break;
             }
             case 7: {
                 // Binary search by rooms count.
                 
                 // Getting the rooms count.
                 instant_fputs("Введите количество комнат в квартирах, информацию о которых вы хотите найти: ", stdout);
                 int apartment_rooms_count_to_find;
                 if (safe_fscanf(stdin, "%d", &apartment_rooms_count_to_find) != 1) {
                     // If something went wrong, terminating the current iteration.
                     fputs("Ошибка ввода. Вероятно, введено некорректное значение.\n", stdout);
                     break;
                 }
                 
                 // Trying to find fitting apartments.
                 size_t found_apartments_by_rooms_count_count;
                 apartment *found_apartments_by_rooms_count = binary_search_by_rooms_count(
                     filename,
                     apartment_rooms_count_to_find,
                     &found_apartments_by_rooms_count_count
                 );
                 
                 if (found_apartments_by_rooms_count_count == (size_t)-1) {
                     // Error.
                     fputs("Произошла ошибка при поиске квартир.\n", stdout);
                 } else if (found_apartments_by_rooms_count_count == 0) {
                     // No such apartments found.
                     printf("Квартир со стоимостью %d не найдено.\n", apartment_rooms_count_to_find);
                 } else {
                     printf("Найденные квартиры с количеством комнат %d:1\n\n", apartment_rooms_count_to_find);
                     for (size_t i = 0; i < found_apartments_by_rooms_count_count; i++) {
                         // Apartments found. Outputting them.
                         if (fprint_apartment(stdout, found_apartments_by_rooms_count[i]) == -1) {
                             // Output failure.
                             fputs("Произошла ошибка при выводе одной из квартир.\n", stdout);
                         }
                         
                         fputs("\n", stdout);
                         
                         if (i < found_apartments_by_rooms_count_count - 1) {
                             // Newline for the apartments that are not last in file.
                             fputs("\n", stdout);
                         }
                     }
                     
                     // Freeing the found apartments.
                     free(found_apartments_by_rooms_count);
                 }
                 
                 break;
             }
             case 8: {
                 // Sorting apartments by cost (quicksort).
                 
                 if (quicksort_by_cost(filename) == 0) {
                     // Success.
                     printf("Файл %s успешно отсортирован по неубыванию стоимости.\n", filename);
                 } else {
                     // Failure.
                     fputs("Возникла ошибка при сортировке файла.\n", stdout);
                 }
                 
                 break;
             }
             case 9: {
                 // Sorting apartments by area (selection sort).
                 
                 if (selection_sort_by_area(filename) == 0) {
                     // Success.
                     printf("Файл %s успешно отсортирован по неубыванию площади квартир.\n", filename);
                 } else {
                     // Failure.
                     fputs("Возникла ошибка при сортировке файла.\n", stdout);
                 }
                 
                 break;
             }
             case 10: {
                 // Sorting apartments by addition date (insertion sort).
                 
                 if (insertion_sort_by_addition_date(filename) == 0) {
                     // Success.
                     printf("Файл %s успешно отсортирован по неубыванию даты добавления.\n", filename);
                 } else {
                     // Failure.
                     fputs("Возникла ошибка при сортировке файла.\n", stdout);
                 }
                 
                 break;
             }
             case 11: {
                 // Searching apartments by cost range and rooms count.
                 
                 // Getting the low price border.
                 instant_fputs("Введите нижнюю границу стоимости квартир, информацию о которых вы хотите найти: ",
                               stdout);
                 float apartment_cost_to_find_low;
                 if (safe_fscanf(stdin, "%f", &apartment_cost_to_find_low) != 1) {
                     // If something went wrong, terminating the current iteration.
                     fputs("Ошибка ввода. Вероятно, введено некорректное значение.\n", stdout);
                     break;
                 }
                 
                 // Similarly.
                 instant_fputs("Введите верхнюю границу стоимости квартир, информацию о которых вы хотите найти: ",
                               stdout);
                 float apartment_cost_to_find_high;
                 if (safe_fscanf(stdin, "%f", &apartment_cost_to_find_high) != 1) {
                     fputs("Ошибка ввода. Вероятно, введено некорректное значение.\n", stdout);
                     break;
                 }
                 
                 // Similarly.
                 instant_fputs("Введите количество комнат в квартирах, информацию о которых вы хотите найти: ", stdout);
                 int apartment_rooms_count_to_find;
                 if (safe_fscanf(stdin, "%d", &apartment_rooms_count_to_find) != 1) {
                     fputs("Ошибка ввода. Вероятно, введено некорректное значение.\n", stdout);
                     break;
                 }
                 
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
                     // Error.
                     fputs("Произошла ошибка при поиске квартир.\n", stdout);
                 } else if (found_apartments_by_criteria_count == 0) {
                     // No such apartments found.
                     printf("Квартир с такими параметрами в файле %s не найдено.\n", filename);
                 } else {
                     // Apartments found. Outputting them.
                     fputs("Квартиры, удовлетворяющие введённым параметрам:\n\n", stdout);
                     for (size_t i = 0; i < found_apartments_by_criteria_count; i++) {
                         if (fprint_apartment(stdout, found_apartments_by_criteria[i]) == -1) {
                             // Output failure.
                             fputs("Произошла ошибка при выводе одной из квартир.\n", stdout);
                         }
                         
                         fputs("\n", stdout);
                         
                         if (i < found_apartments_by_criteria_count - 1) {
                             // Newline for the apartments that are not last in file.
                             fputs("\n", stdout);
                         }
                     }
                     
                     // Freeing the array of found apartments.
                     free(found_apartments_by_criteria);
                 }
                 
                 break;
             }
             case 12: {
                 // Searching newest free apartments.
                 
                 // Entering the earliest date of addition.
                 instant_fputs("Введите дату, после которой должны были быть добавлены искомые квартиры: ", stdout);
                 date apartment_addition_date_to_find;
                 if (fscan_date(stdin, &apartment_addition_date_to_find) == -1) {
                     // If failed, terminating the current iteration.
                     fputs("Ошибка ввода. Вероятно, введено некорректное значение.\n", stdout);
                     break;
                 }
                 
                 // Trying to find fitting apartments.
                 size_t found_apartments_by_addition_date_count;
                 apartment *found_apartments_by_addition_date = search_newest_free_apartments(
                     filename,
                     apartment_addition_date_to_find,
                     &found_apartments_by_addition_date_count
                 );
                     
                 if (found_apartments_by_addition_date_count == (size_t)-1) {
                     // Error.
                     fputs("Произошла ошибка при поиске квартир.\n", stdout);
                 } else if (found_apartments_by_addition_date_count == 0) {
                     // No such apartments found.
                     fputs("Свободных квартир добавленных после введённой даты не найдено.\n", stdout);
                 } else {
                     // Apartments found. Outputting them.
                     fputs("Найденные свободные квартиры, появившиеся в базе после определённой даты:\n\n", stdout);
                     for (size_t i = 0; i < found_apartments_by_addition_date_count; i++) {
                         if (fprint_apartment(stdout, found_apartments_by_addition_date[i]) == -1) {
                             // Output failure.
                             fputs("Произошла ошибка при выводе одной из квартир.\n", stdout);
                         }
                         
                         fputs("\n", stdout);
                         
                         if (i < found_apartments_by_addition_date_count - 1) {
                             // Newline for the apartments that are not last in file.
                             fputs("\n", stdout);
                         }
                     }
                     
                     // Freeing the array of found apartments.
                     free(found_apartments_by_addition_date);
                 }
                 
                 break;
             }
             default: {
                 break;
             }
         }
     }
     
     // The farewell message.
     instant_fputs("Работа программы завершена.\n", stdout);
     
     return 0;
 }