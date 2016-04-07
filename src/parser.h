#include <string.h>

class container_object;

#define MAX_BUFFER_SIZE 64

class command
{
    public:
        command(){ memset(m_name, '\0', MAX_BUFFER_SIZE); }
        void populate(container_object* obj);
        const char* get_name(){ return m_name; }
        virtual void populate_specific(container_object* obj) = 0;

    private:
        char m_name[MAX_BUFFER_SIZE];
};

class set_command: public command
{
    public:
        set_command():
            m_direction(0),
            m_gear(0),
            m_magnitude(0),
            m_lights(false)
        { memset(m_direction_string, '\0', MAX_BUFFER_SIZE); }
        void populate_specific(container_object* obj);
        long get_direction(){ return m_direction; };
        long get_gear(){ return m_gear; };
        long get_magnitude(){ return m_direction; };
        const char* get_direction_string(){ return m_direction_string; };
        bool get_lights(){ return m_lights; };

    private:
        long m_direction;
        long m_gear;
        long m_magnitude;
        char m_direction_string[MAX_BUFFER_SIZE];
        bool m_lights;

};
