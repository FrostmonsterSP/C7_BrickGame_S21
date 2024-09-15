/**
 * @file interface.h
 * @brief Файл с описанием функций и типов, связанных с интерфейсом
 *
 * Данный файл содержит объявления типов и функций, которые
 * непосредственно связаны с интерфейсом. Он включает в себя
 * функцию @link interface interface@endlink, которая
 * начинает интерфейс, и функции @link startInterface startInterface@endlink
 * и @link endInterface endInterface@endlink, которые
 * соответственно запускают и останавливают интерфейс.
 *
 * @note Только данный файл из файлов интерфейса должен быть подключен к main.c
 * напрямую.
 */
#ifndef INTERFACE_H
#define INTERFACE_H

int interface(void);
void startInterface(void);
void endInterface(void);

#endif  // INTERFACE_H
