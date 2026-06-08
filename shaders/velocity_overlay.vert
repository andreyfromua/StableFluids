#version 410 core

// Draws an instanced MAC-grid debugging overlay without allocating geometry.
// Pass 0 emits the coarse sample grid. Pass 1 emits one component arrow at
// every unique sample-grid face: u on vertical faces and v on horizontal
// faces. Thus every sample cell has left/right and bottom/top face arrows
// without drawing shared interior faces twice.

uniform sampler2D u_u;
uniform sampler2D u_v;
uniform ivec2     u_grid_size;
uniform ivec2     u_sample_count;
uniform ivec2     u_viewport_size;
uniform int       u_stride;
uniform int       u_pass;
uniform float     u_vector_scale;

out vec4 v_color;

vec2 grid_to_pixel(vec2 position) {
    return position / vec2(u_grid_size) * vec2(u_viewport_size);
}

vec2 pixel_to_clip(vec2 position) {
    return 2.0 * position / vec2(u_viewport_size) - 1.0;
}

vec2 line_vertex(vec2 start, vec2 end, float width, int vertex_id) {
    vec2 direction = end - start;
    float segment_length = length(direction);
    if (segment_length < 1e-4) {
        return vec2(-2.0);
    }

    direction /= segment_length;
    vec2 normal = vec2(-direction.y, direction.x) * (0.5 * width);

    const int corner_ids[6] = int[6](0, 1, 2, 2, 1, 3);
    int corner = corner_ids[vertex_id];
    vec2 along = (corner >= 2) ? end : start;
    float side = ((corner & 1) == 0) ? -1.0 : 1.0;
    return along + side * normal;
}

vec2 arrow_vertex(vec2 tail, vec2 tip, int vertex_id) {
    vec2 direction = tip - tail;
    float arrow_length = length(direction);
    if (arrow_length < 0.75) {
        return vec2(-2.0);
    }

    direction /= arrow_length;
    vec2 normal = vec2(-direction.y, direction.x);
    float head_length = min(8.0, 0.45 * arrow_length);
    float head_width = min(7.0, 0.40 * arrow_length);
    vec2 head_base = tip - direction * head_length;

    if (vertex_id < 6) {
        return line_vertex(tail, head_base, 2.0, vertex_id);
    }

    int head_vertex = vertex_id - 6;
    if (head_vertex == 0) {
        return tip;
    }
    return head_base +
           (head_vertex == 1 ? head_width : -head_width) * normal;
}

void main() {
    vec2 pixel_position;

    if (u_pass == 0) {
        int vertical_count = u_sample_count.x + 1;
        vec2 start;
        vec2 end;

        if (gl_InstanceID < vertical_count) {
            int line = gl_InstanceID;
            float x = float(min(line * u_stride, u_grid_size.x));
            start = vec2(x, 0.0);
            end = vec2(x, float(u_grid_size.y));
        } else {
            int line = gl_InstanceID - vertical_count;
            float y = float(min(line * u_stride, u_grid_size.y));
            start = vec2(0.0, y);
            end = vec2(float(u_grid_size.x), y);
        }

        pixel_position = line_vertex(grid_to_pixel(start),
                                     grid_to_pixel(end),
                                     1.0, gl_VertexID);
        v_color = vec4(0.20, 0.62, 0.78, 0.42);
    } else {
        vec2 tail_grid;
        vec2 velocity;
        float block_extent;

        int horizontal_face_count =
            (u_sample_count.x + 1) * u_sample_count.y;
        if (gl_InstanceID < horizontal_face_count) {
            int face_x = gl_InstanceID % (u_sample_count.x + 1);
            int sample_y = gl_InstanceID / (u_sample_count.x + 1);
            int face_i = min(face_x * u_stride, u_grid_size.x);
            int y0 = sample_y * u_stride;
            int y1 = min(y0 + u_stride, u_grid_size.y);
            int center_j =
                clamp((y0 + y1 - 1) / 2, 0, u_grid_size.y - 1);
            float u = texelFetch(u_u, ivec2(face_i, center_j), 0).r;
            tail_grid = vec2(float(face_i), float(center_j) + 0.5);
            velocity = vec2(u, 0.0);
            int left_width =
                face_x > 0
                    ? face_i - (face_x - 1) * u_stride
                    : 0;
            int right_width =
                face_x < u_sample_count.x
                    ? min((face_x + 1) * u_stride, u_grid_size.x) - face_i
                    : 0;
            int adjacent_width = max(left_width, right_width);
            if (left_width > 0 && right_width > 0) {
                adjacent_width = min(left_width, right_width);
            }
            block_extent = float(min(y1 - y0, adjacent_width));
            v_color = vec4(0.12, 0.92, 1.00, 0.92);
        } else {
            int local_instance = gl_InstanceID - horizontal_face_count;
            int sample_x = local_instance % u_sample_count.x;
            int face_y = local_instance / u_sample_count.x;
            int x0 = sample_x * u_stride;
            int x1 = min(x0 + u_stride, u_grid_size.x);
            int center_i =
                clamp((x0 + x1 - 1) / 2, 0, u_grid_size.x - 1);
            int face_j = min(face_y * u_stride, u_grid_size.y);
            float v = texelFetch(u_v, ivec2(center_i, face_j), 0).r;
            tail_grid = vec2(float(center_i) + 0.5, float(face_j));
            velocity = vec2(0.0, v);
            int bottom_height =
                face_y > 0
                    ? face_j - (face_y - 1) * u_stride
                    : 0;
            int top_height =
                face_y < u_sample_count.y
                    ? min((face_y + 1) * u_stride, u_grid_size.y) - face_j
                    : 0;
            int adjacent_height = max(bottom_height, top_height);
            if (bottom_height > 0 && top_height > 0) {
                adjacent_height = min(bottom_height, top_height);
            }
            block_extent = float(min(x1 - x0, adjacent_height));
            v_color = vec4(1.00, 0.64, 0.12, 0.92);
        }

        float max_length = max(0.35, 0.45 * block_extent);
        velocity *= u_vector_scale;
        velocity = clamp(velocity, vec2(-max_length), vec2(max_length));

        vec2 tail_pixel = grid_to_pixel(tail_grid);
        vec2 tip_pixel = grid_to_pixel(tail_grid + velocity);
        pixel_position = arrow_vertex(tail_pixel, tip_pixel, gl_VertexID);
    }

    gl_Position = vec4(pixel_to_clip(pixel_position), 0.0, 1.0);
}
