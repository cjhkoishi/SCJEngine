#include "Grid.h"

void Grid::construct(int num_W, int num_H)
{
	W = H = 1000 + 10;
	X = Y = -5;
	this->num_W = num_W;
	this->num_H = num_H;
	double wl = W / num_W, hl = H / num_H;
	data.resize(num_W * num_H);

	for (int i = 0; i < container->balls.size(); i++) {
		auto& ball = container->balls[i];
		dvec2 AA = ball.pos - dvec2(ball.r);
		dvec2 BB = ball.pos + dvec2(ball.r);
		ivec2 cell_AA((int)((AA.x - X) / wl), (int)((AA.y - Y) / hl));
		ivec2 cell_BB((int)((BB.x - X) / wl), (int)((BB.y - Y) / hl));
		for (int xc = cell_AA.x; xc <= cell_BB.x; xc++) {
			for (int yc = cell_AA.y; yc <= cell_BB.y; yc++) {
				data[xc + yc * num_W].obj_indices.push_back(i);
			}
		}
	}

	vector<vec2> pts((num_W + 1) * (num_H + 1));

	for (int i = 0; i <= num_H; i++) {
		for (int j = 0; j <= num_W; j++) {
			pts[i * (num_W + 1) + j] = vec2(X + j * wl, Y + i * hl);
		}
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, pts.size() * sizeof(vec2), pts.data(), GL_STATIC_DRAW);


	vector<uvec2> edges(num_W + num_H + 2);
	for (int i = 0; i <= num_H; i++) {
		edges[i] = uvec2(i * (num_W + 1), i * (num_W + 1) + num_W);
	}
	for (int j = 0; j <= num_W; j++) {
		edges[j + num_H + 1] = uvec2(j, j + num_H * (num_W + 1));
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, edges.size() * sizeof(uvec2), edges.data(), GL_STATIC_DRAW);
}

void Grid::start()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);


	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glGenBuffers(1, &CELLEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CELLEBO);

	shader = shader_asset["default"];

	construct(50, 40);
}

void Grid::Bresenham()
{
	activated_cells.clear();
	double wl = W / num_W, hl = H / num_H;
	bool is_swap = abs(ray->dir.y * wl) > abs(hl * ray->dir.x);
	set<int> ball_indices;

	if (is_swap) {
		swap(wl, hl);
		swap(X, Y);
		swap(W, H);
		swap(ray->dir.x, ray->dir.y);
		swap(ray->pos.x, ray->pos.y);
		swap(num_W, num_H);
	}
	bool is_rev = ray->dir.x < 0;
	auto handle_cell = [&](ivec2 cell) {
		if (is_swap)
			swap(cell[0], cell[1]);
		activated_cells.push_back(cell);
		for (auto& index : data[cell[0] + cell[1] * (is_swap ? num_H : num_W)].obj_indices) {
			ball_indices.insert(index);
		}
	};

	double k = ray->dir.y / ray->dir.x;
	if (is_rev)
		k = -k;

	double t;
	bool res = ray->RayHitAABB(dvec2(X, Y), dvec2(X + W, Y + H), &t);
	if (res) {

		double x0 = ray->pos.x + t * ray->dir.x - X;
		double y0 = ray->pos.y + t * ray->dir.y - Y;



		int xc = clamp<int>((int)((x0) / wl), 0, num_W - 1);
		int yc = clamp<int>((int)((y0) / hl), 0, num_H - 1);

		double yr = y0 - yc * hl;
		//处理第一个格子	
		double dist = abs((xc + (is_rev ? 0 : 1)) * wl - x0);
		yr += dist * k;
		bool c1 = yr > hl, c0 = yr < 0;
		if (c1 || c0) {
			c1 ? yr -= hl : yr += hl;
			if (xc >= 0 && yc >= 0 && xc < num_W && yc < num_H)
				handle_cell(ivec2(xc, yc));
			c1 ? yc++ : yc--;
			if (yc >= 0 && yc < num_H) {
				handle_cell(ivec2(xc, yc));
				is_rev ? xc-- : xc++;
			}
		}
		else {
			if (xc >= 0 && yc >= 0 && xc < num_W && yc < num_H) {
				handle_cell(ivec2(xc, yc));
				is_rev ? xc-- : xc++;
			}
		}
		//bresenham算其余格子
		double dy = k * wl;
		while (true) {
			yr += dy;
			bool c1 = yr > hl, c0 = yr < 0;
			if (c1 || c0) {
				c1 ? yr -= hl : yr += hl;
				if (xc < 0 || yc < 0 || xc >= num_W || yc >= num_H)
					break;
				handle_cell(ivec2(xc, yc));
				c1 ? yc++ : yc--;
				if (yc < 0 || yc >= num_H)
					break;
				handle_cell(ivec2(xc, yc));
				is_rev ? xc-- : xc++;
			}
			else {
				if (xc < 0 || yc < 0 || xc >= num_W || yc >= num_H)
					break;
				handle_cell(ivec2(xc, yc));
				is_rev ? xc-- : xc++;
			}
		}
	}
	if (is_swap) {
		swap(wl, hl);
		swap(X, Y);
		swap(W, H);
		swap(ray->dir.x, ray->dir.y);
		swap(ray->pos.x, ray->pos.y);
		swap(num_W, num_H);
	}

	int count = 0;
	for (auto& index : ball_indices) {
		auto& ball = container->balls[index];
		count++;
		if (ray->RayHitBall(ball.pos, ball.r))
			ball.activated = true;
	}
	cout << count << endl;
}

void Grid::render(const mat4& view, const mat4& proj)
{
	shader.use();
	shader.setMat4("model", getObject()->getWorldTransform());
	shader.setMat4("view", view);
	shader.setMat4("projection", proj);


	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CELLEBO);
	for (auto& item : activated_cells) {
		unsigned edge[]{
			item.y * (num_W + 1) + item.x,
			item.y * (num_W + 1) + item.x + 1,
			(item.y + 1) * (num_W + 1) + item.x + 1,
			(item.y + 1) * (num_W + 1) + item.x
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(edge), edge, GL_DYNAMIC_DRAW);
		shader.setVec4("color", vec4(1, 0, 0, 0.2));
		glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_INT, 0);
	}
	shader.setVec4("color", vec4(0, 1, 0, 1));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glDrawElements(GL_LINES, 2 * (num_W + num_H + 2), GL_UNSIGNED_INT, 0);

}

void Grid::update()
{
	Bresenham();

	Renderer::update();
}
