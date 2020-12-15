/******************************************************************************/
/*                       Header includes                                      */

#include <curses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>

#include "alarm.h"
#include "control.h"
#include "mqtt.h"

/******************************************************************************/
/*! @file window.c
 * @brief Inteface to window
 */
#include "window.h"

/****************************************************************************/
/*!                        Global Statements                                */

Windows windows;
WINDOW *menu_bar;

DeviceData device_data[MAX_DEVICE] = {0};
int num_device = 0;

sem_t output_semaphore;
extern char * mac_address;

const char *key_function[] =
{
    " F2 ",
    " F3 ",
    " F4 ",
    " F5 ",
    " F6 ",
    " F7 ",
    " F8 ",
    " F9 "
};

const char *message[] = 
{
    "on",
    "off",
    "open",
    "closed",
    "enabled",
    "disabled"
};

const char *room_option[] =
{
    " Bedroom 1 ",
    " Bedroom 2 ",
    " Bedroom 3 ",
    "Bath room 1",
    "Bath room 2",
    "Bath room 3",
    "Living room",
    "  Kitchen  ",
    "   Yard    "
};

const char *room_opt[] =
{
    "Bedroom1",
    "Bedroom2",
    "Bedroom3",
    "Bathroom1",
    "Bathroom2",
    "Bathroom3",
    "Livingroom",
    "Kitchen",
    "Yard"
};

/****************************************************************************/
/*!                         Functions                                       */


/*!
 * @brief Function used to create the lamps button.
 */
void create_button_lamp ()
{
    for (int i = 1; i <= NLAMP; i++)
    {
        wmove(menu_bar,0,i*BUTTON_SIZE);
        char string[] = "  lamp x   ";
        string[7] = '0'+i;
        wattron(menu_bar,COLOR_PAIR(2));
        waddstr(menu_bar, string);
        wattron(menu_bar,COLOR_PAIR(3));
        waddstr(menu_bar, key_function[i]);
        wattroff(menu_bar,COLOR_PAIR(3));
    }
} 

/*!
 * @brief Function used to create the alarm button.
 */
void create_button_alarm()
{
    wmove(menu_bar,0,0);
    wattron(menu_bar,COLOR_PAIR(2));
    waddstr(menu_bar, "   alarm   ");
    wattron(menu_bar,COLOR_PAIR(3));
    waddstr(menu_bar, key_function[0]);
    wattroff(menu_bar,COLOR_PAIR(3));
}

/*!
 * @brief Function used to create the device button.
 */
void create_button_device(int pos)
{
    wmove(menu_bar,0,(NLAMP+num_device)*BUTTON_SIZE);
    wattron(menu_bar,COLOR_PAIR(2));
    waddstr(menu_bar, room_option[pos]);
    wattron(menu_bar,COLOR_PAIR(3));
    waddstr(menu_bar, key_function[NLAMP+num_device]);
    wattroff(menu_bar,COLOR_PAIR(3));
    touchwin(stdscr);
    refresh();
}

/*!
 * @brief Function used to draw the switch.
 */
void switch_draw(bool turn, unsigned char key)
{
    wmove(menu_bar,FIRST_LINE_MENU,key*BUTTON_SIZE+11);
    if (turn)
    {
        wattron(menu_bar,COLOR_PAIR(4));
        waddstr(menu_bar, key_function[key]);
        wattroff(menu_bar,COLOR_PAIR(4));
    }
    else
    {
        wattron(menu_bar,COLOR_PAIR(3));
        waddstr(menu_bar, key_function[key]);
        wattroff(menu_bar,COLOR_PAIR(3));
    }
    touchwin(stdscr);
    refresh();
}

/*!
 * @brief Function used to switch the lamps.
 */ 
bool switch_button(unsigned char key, SystemData *system_data)
{
    bool auxiliary;
    
    system_data->lamp ^= (1<<(key-1));
    auxiliary = system_data->lamp & (1<<(key-1));
    
    switch_draw(auxiliary, key);

    return auxiliary;
}

/*!
 * @brief Function used to switch the devices.
 */ 
bool switch_device(unsigned char key, int id)
{    
    device_data[id].status = !device_data[id].status;
    
    switch_draw(device_data[id].status, key);

    return device_data[id].status;
}

/*!
 * @brief Function used to switch the alarm.
  */ 
void switch_alarm (unsigned char key, SystemData *system_data, char *buffer)
{
    if (!system_data->alarm && alarm_control(system_data))
    {
        sprintf(buffer, "close doors and windows before activating the alarm!");
    }
    else
    {
        if (system_data->alarm_pid)
        {
            kill(system_data->alarm_pid, SIGKILL);
            system_data->alarm_pid = 0;
        }
        system_data->alarm = !(system_data->alarm);

        switch_draw(system_data->alarm, key);

        sprintf(buffer, "alarm %s", message[system_data->alarm ? 0 : 1]);
    }
}

/*!
 * @brief Function used to create the menu bar
 */  
void create_menu_bar()
{
    menu_bar=subwin(stdscr,NLINES_MENU, COLS, FIRST_LINE_MENU, FIRST_COLUMN_MENU);
    create_button_alarm();
    create_button_lamp();

    windows.message=subwin(stdscr,1,BUTTON_SIZE*5,23,1);
    refresh();
}

/*!
 * @brief Function used to print the commands
 */ 
void print_commands(int line)
{
    move(line,1);
    printw("Commands:");
    move(++line,4);
    printw("F2: turn on or off kitchen lamp");
    move(++line,4);
    printw("F3: turn on or off room lamp");
    move(++line,4);
    printw("F4: turn on or off bedroom 1 lamp");
    move(++line,4);
    printw("F5: turn on or off bedroom 2 lamp");
    move(++line,4);
    printw("F6: set AC temperature or press Del to turn off");
    move(++line,4);
    printw("F7: turn on or off alarm");
    move(++line,5);
    printw("^: up");
    move(++line,5);
    printw("v: down");
    move(++line,1);
    printw("Enter: select");
    move(++line,3);
    printw("Esc: exit");
    refresh();
}

/*!
 * @brief Function used to print the instructions
 */ 
void print_instructions(int line)
{
    move(line,1);
    printw("Instructions:");
    move(++line,3);
    printw("red: switched off");
    move(++line,1);
    printw("green: switched on");
}

/*!
 * @brief Function used to draws menu items when the F3 or F4 keys are pressed
 */ 
WINDOW **create_items()
{
    int column = (NLAMP+num_device)*BUTTON_SIZE;
    int number_items = sizeof(room_option)/sizeof(room_option[0]);
    WINDOW **menu_items;
    menu_items=(WINDOW **)malloc((number_items+1)*sizeof(WINDOW *));
    menu_items[0]=newwin(number_items+2,BUTTON_SIZE-2,1,column);

    wbkgd(menu_items[0],COLOR_PAIR(2));
    box(menu_items[0],ACS_VLINE,ACS_HLINE);

    for (int i = 1; i <= number_items; i++)
    {
        menu_items[i]=subwin(menu_items[0],1,BUTTON_SIZE-4,i+1,column+1);
    }

    for (int i = 0; i < number_items; i++)
    {
        wprintw(menu_items[i+1]," %s ", room_option[i]);
    }
    wbkgd(menu_items[1],COLOR_PAIR(1));
    wrefresh(menu_items[0]);
    return menu_items;
}

/*!
 * @brief Function that delete the menu items 
 */ 
void deleteMenuItems(WINDOW **menu_items)
{
    int number_items = sizeof(room_option)/sizeof(room_option[0]);
    for (int i = 0; i <= number_items; i++)
    {
        delwin(menu_items[i]);
    }
    free(menu_items);
}


// /*!
//  * @brief Function used to scroll
//  */ 
int scrollmenu(WINDOW **menu_items) 
{
    int number_items = sizeof(room_option)/sizeof(room_option[0]);
    int key;
    int selected=0;
    while (1)
    {
        key=getch();
        if (key==KEY_DOWN || key==KEY_UP)
        {
            wbkgd(menu_items[selected+1],COLOR_PAIR(2));
            wnoutrefresh(menu_items[selected+1]);
            if (key==KEY_DOWN)
            {
                selected=(selected+1) % number_items;
            } 
            else
            {
                selected=(selected+number_items-1) % number_items;
            }
            wbkgd(menu_items[selected+1],COLOR_PAIR(1));
            wnoutrefresh(menu_items[selected+1]);
            doupdate();
        }
        else if (key==ENTER)
        {
            return selected;
        }
    }
}

/*!
 * @brief Function that create the menu 
 */ 
void menu ()
{
    create_menu_bar();
    print_commands(FIRST_LINE_COMMANDS);
    print_instructions(FIRST_LINE_INSTRUCTIONS);
}

/*!
 * @brief Function that initialize the window 
 */ 
void initialize_window ()
{
    initscr();      //Init the ncurses
    start_color();

    /* Definition color pairs*/
    init_pair(1,COLOR_CYAN,COLOR_BLACK);
    init_pair(2,COLOR_BLACK,COLOR_CYAN);
    init_pair(3,COLOR_WHITE,COLOR_RED);  
    init_pair(4,COLOR_WHITE,COLOR_GREEN);  
    init_pair(5,COLOR_WHITE,COLOR_BLUE);  

    cbreak(); // Disable line buffering, gimme every thing
    curs_set(0);  // invisible cursor
    noecho();   
    keypad(stdscr,TRUE);  // active the function keypads 
    bkgd(COLOR_PAIR(1));

    menu();

    sem_init(&output_semaphore, 0, 1);
}

void add_device ()
{
    WINDOW **room_items;
    char topic[100];
    char msg[60];
    alarm(0);
    sprintf(device_data[num_device].mac, "%s", mac_address);
    num_device++;
    room_items = create_items();
    int item = scrollmenu(room_items);
    device_data[num_device-1].room = (char *)room_opt[item];
    deleteMenuItems(room_items);
    create_button_device(item);
    subscribe(device_data[num_device-1].room);
    sprintf(topic,"%s/%s", MQTT_BASE_TOPIC, mac_address);
    sprintf(msg,"{\"id\":%d, \"room\":\"%s\"}", num_device-1, device_data[num_device-1].room);
    publish(topic, msg);
    mac_address = NULL;
    alarm(1);
}

/*!
 *  @brief Thread function to user input 
 */
void *input_values (void *args)
{
    SystemData *system_data = (SystemData *) args;
    int key;
    bool auxiliary;
    char buffer[60];

    while (1)
    {
        key = getch();

        sem_wait(&output_semaphore);

        werase(windows.message);
        wrefresh(windows.message);
        switch (key)
        {
        case KEY_F(10):
            if (num_device < MAX_DEVICE && mac_address)
            {
                add_device();
                sprintf(buffer,"New device added.");
            }
            else if (num_device == MAX_DEVICE)
            {
                sprintf(buffer,"Impossible to add more than %d device(s).", MAX_DEVICE);
            }
            else
            {
                sprintf(buffer,"Connect a new device!");
            }
            break;
        case KEY_F(2):
            switch_alarm((unsigned char)0, system_data, buffer);
            break;
        case KEY_F(3):
            auxiliary = switch_button((unsigned char)1, system_data);
            sprintf(buffer, "kitchen lamp %s", message[auxiliary ? 0 : 1]);
            break;
        case KEY_F(4):
            auxiliary = switch_button((unsigned char)2, system_data);
            sprintf(buffer, "room lamp %s", message[auxiliary ? 0 : 1]);
            break;
        case KEY_F(5):
            if (strlen(device_data[0].mac))
            {
                auxiliary = switch_device((unsigned char)3, 0);
                sprintf(buffer, "%s device %s", device_data[0].room, message[auxiliary ? 0 : 1]);
                push(); 
            }
            break;
        case KEY_F(6):
            if (strlen(device_data[1].mac))
            {
                auxiliary = switch_device((unsigned char)4, 1);
                sprintf(buffer, "%s device %s", device_data[1].room, message[auxiliary ? 0 : 1]);
                push(); 
            }
            break;
        case KEY_F(7):
            if (strlen(device_data[2].mac))
            {
                auxiliary = switch_device((unsigned char)5, 2);
                sprintf(buffer, "%s device %s", device_data[2].room, message[auxiliary ? 0 : 1]);
                push(); 
            }
            break;
        case KEY_F(8):
            if (strlen(device_data[3].mac))
            {
                auxiliary = switch_device((unsigned char)6, 3);
                sprintf(buffer, "%s device %s", device_data[3].room, message[auxiliary ? 0 : 1]);
                push(); 
            }
            break;
        case KEY_F(9):
            if (strlen(device_data[4].mac))
            {
                auxiliary = switch_device((unsigned char)7, 4);
                sprintf(buffer, "%s device %s", device_data[4].room, message[auxiliary ? 0 : 1]);
                push(); 
            }
            break;
        case ESCAPE:
            sprintf(buffer, "exit");
            store_data(buffer);
            return NULL;
        }
        
        wprintw(windows.message, buffer);
        touchwin(stdscr);
        refresh();

        sem_post(&output_semaphore);

        store_data(buffer);
    }
}

/*!
 *  @brief Thread function that shows the values ​​on the screen
 */
void *output_values (void *args)
{
    sem_wait(&output_semaphore);
    SystemData *system_data = (SystemData *) args;
    int line = 2;

    move(line,BUTTON_SIZE*5);
    move(++line,BUTTON_SIZE*5);
    printw("Temperature: %.2f oC  ", system_data->temperature++);
    move(++line,BUTTON_SIZE*5);
    printw("Humidity: %.2f %%  ", system_data->humidity);

    line += 2;
    
    move(line,BUTTON_SIZE*5);
    printw("Living room");
    move(++line,BUTTON_SIZE*5+2);
    printw("Presence Sensor: %s  ", message[4+(system_data->presence_sensor&1 ? 0 : 1)]);
    move(++line,BUTTON_SIZE*5+2);
    printw("Window: %s  ", message[2+(system_data->open_sensor&(1<<3) ? 0 : 1)]);
    move(++line,BUTTON_SIZE*5+2);
    printw("Door: %s  ", message[2+(system_data->open_sensor&(1<<2) ? 0 : 1)]);
    
    line += 2;
    move(line,BUTTON_SIZE*5);
    printw("Kitchen");
    move(++line,BUTTON_SIZE*5+2);
    printw("Presence Sensor: %s  ", message[4+(system_data->presence_sensor&(1<<1) ? 0 : 1)]);
    move(++line,BUTTON_SIZE*5+2);
    printw("Window: %s  ", message[2+(system_data->open_sensor&(1<<1) ? 0 : 1)]);
    move(++line,BUTTON_SIZE*5+2);
    printw("Door: %s  ", message[2+(system_data->open_sensor&1 ? 0 : 1)]);
    
    for (size_t i = 0; i < num_device; i++)
    {
        line += 2;
        move(line,BUTTON_SIZE*5);
        printw(device_data[i].room);
        move(++line,BUTTON_SIZE*5+2);
        printw("Temperature: %d.00 oC  ", device_data[i].temperature);
        move(++line,BUTTON_SIZE*5+2);
        printw("Humidity: %d.00 %%  ", device_data[i].humidity);
        move(++line,BUTTON_SIZE*5+2);
        printw("status: %d ", device_data[i].status);
    }
    
    refresh();

    sem_post(&output_semaphore);

    return NULL;
}

/*!
 *  @brief Thread function that ends the window
 */
void end_window() 
{
    delwin(windows.menu_bar);
    delwin(windows.message);
    endwin();
}