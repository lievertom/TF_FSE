#ifndef WINDOW_H_
#define WINDOW_H_

/******************************************************************************/
/*                       Header includes                                      */
#include <curses.h>

/****************************************************************************/
/*!                              Macros                                     */

#define ENTER                   10
#define ESCAPE                  27
 
#define NTEMPERATURE_VALUES     21
#define TEMPERATURE_FIRST_VALUE 10

#define NLINES_MENU             1
#define NCOLUMNS_MENU           80
#define FIRST_LINE_MENU         0
#define FIRST_COLUMN_MENU       0
#define FIRST_LINE_COMMANDS     2
#define FIRST_LINE_INSTRUCTIONS 15

#define BUTTON_SIZE             17

/****************************************************************************/
/*!                            Structs                                      */
typedef struct Windows
{
    WINDOW *menu_bar;
    WINDOW *message;
} Windows;

/****************************************************************************/
/*!                         Functions                                       */

/*!
 *  @brief function that initialize window
 *
 *  @return void
 *
 */
void initialize_window ();

/*!
 *  @brief Thread function that shows the values ​​on the screen
 *
 * @param[in] args       : Pointer to access the data structure
 *
 *  @return void
 *
 */
void *output_values (void *args);

/*!
 *  @brief Thread function to user input 
 *
 * @param[in] args       : Pointer to access the data structure
 *
 *  @return void
 *
 */
void *input_values (void *args);

/*!
 *  @brief Thread function that ends the window
 *
 *  @return void
 *
 */
void end_window(); 

#endif /* WINDOW_H_ */