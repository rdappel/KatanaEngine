#pragma once

#include <memory>

#include <allegro5/allegro.h>

namespace KatanaEngine
{
    struct DisplayDeleter
    {
        void operator()(ALLEGRO_DISPLAY* display) const
        {
            if (display) al_destroy_display(display);
        }
    };

    struct EventQueueDeleter
    {
        void operator()(ALLEGRO_EVENT_QUEUE* queue) const
        {
            if (queue) al_destroy_event_queue(queue);
        }
    };

    struct TimerDeleter
    {
        void operator()(ALLEGRO_TIMER* timer) const
        {
            if (timer) al_destroy_timer(timer);
        }
    };

    using DisplayPtr = std::unique_ptr<ALLEGRO_DISPLAY, DisplayDeleter>;
    using EventQueuePtr = std::unique_ptr<ALLEGRO_EVENT_QUEUE, EventQueueDeleter>;
    using TimerPtr = std::unique_ptr<ALLEGRO_TIMER, TimerDeleter>;

    struct PlatformContext
    {
        DisplayPtr pDisplay{};
        EventQueuePtr pEventQueue{};
        TimerPtr pTimer{};
    };
}
