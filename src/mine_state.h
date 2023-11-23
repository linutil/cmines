#pragma once

typedef enum {
    MS_FREE,
    MS_MINE,
    MS_FLAGGED_NEGATIVE,
    MS_FLAGGED_POSITIVE,
    MS_CHECKED_FREE,
    MS_CHECKED_MINE,
} mine_state;