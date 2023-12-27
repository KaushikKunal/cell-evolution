import pygame
import sys
import random

NUM_SPECIES = 5
NUM_BLOBS = 150
MAX_FORCE = 0.1
MAX_DIST = 300
FRICTION = 0.9

pygame.init()
clock = pygame.time.Clock()
pygame.font.init()
font = pygame.font.SysFont('Arial', 20)

# matrix of random floats between -0.1 and 0.1
rule_matrix = [[0]*NUM_SPECIES] * NUM_SPECIES
species_colors = [(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)) for i in range(NUM_SPECIES)]

def generate_rules():
    global rule_matrix
    rule_matrix = [[random.random() * 2*MAX_FORCE - MAX_FORCE for i in range(NUM_SPECIES)] for j in range(NUM_SPECIES)]

def generate_colors():
    global species_colors
    species_colors = [(random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)) for i in range(NUM_SPECIES)]

# visualize the matrix of rules in the top right corner
def draw_rules(screen, rule_matrix, pos, size=10):
    for i in range(NUM_SPECIES):
        for j in range(NUM_SPECIES):
            # get color based on rule value
            # -MAX_FORCE -> (0, 0, 255)
            # MAX_FORCE -> (255, 255, 255)
            c = int((rule_matrix[i][j] + MAX_FORCE) / (2*MAX_FORCE) * 255)
            pygame.draw.rect(screen, (c, 255-c, 0, 200), (pos[0] + i*size, pos[1] + j*size, size, size))

class Blob:
    def __init__(self, x, y, size, species):
        self.x = x
        self.y = y
        self.size = size
        self.species = species

        self.xvel = 0
        self.yvel = 0

    def interact(self, blob):
        if blob == self:
            return
        
        dx = blob.x - self.x
        dy = blob.y - self.y
        dist = (dx**2 + dy**2)**0.5
        if dist == 0:
            return
        dx /= dist
        dy /= dist
        min_dist = self.size + blob.size + 10
        peak_force = rule_matrix[self.species][blob.species]  # force at the midpoint
        if dist < min_dist:
            # 0 dist -> -MAX_FORCE
            # min_dist -> 0
            force = MAX_FORCE * (dist / min_dist) - MAX_FORCE
        elif dist < (min_dist + MAX_DIST) / 2:
            # min_dist -> 0
            # (min_dist + MAX_DIST) / 2 -> peak_force
            force = peak_force * (dist - min_dist) / ((min_dist + MAX_DIST) / 2 - min_dist)
        elif dist < MAX_DIST:
            # (min_dist + MAX_DIST) / 2 -> peak_force
            # MAX_DIST -> 0
            force = peak_force * (MAX_DIST - dist) / (MAX_DIST - (min_dist + MAX_DIST) / 2)
        else:
            return
            
        self.xvel += force * dx
        self.yvel += force * dy
        

    def interactions(self, blobs):
        for blob in blobs:
            self.interact(blob)
    
    def update(self):
        self.x += self.xvel
        self.y += self.yvel
        self.xvel *= FRICTION
        self.yvel *= FRICTION
        # bounce off walls
        if self.x < 0:
            self.x = 0
            self.xvel = -self.xvel
        elif self.x > 800:
            self.x = 800
            self.xvel = -self.xvel
        if self.y < 0:
            self.y = 0
            self.yvel = -self.yvel
        elif self.y > 600:
            self.y = 600
            self.yvel = -self.yvel


    def draw(self, screen):
        pygame.draw.circle(screen, species_colors[self.species], (self.x, self.y), self.size)



screen = pygame.display.set_mode([800, 600])

pygame.display.set_caption('Life v0.1')

running = True
blobs = []
for i in range(NUM_BLOBS):
    blobs.append(Blob(random.randint(0, 800), random.randint(0, 600), 5, random.randint(0, NUM_SPECIES-1)))
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
            break
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1:
                # move blob to mouse position
                blobs[0].x = event.pos[0]
                blobs[0].y = event.pos[1]
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_r:
                generate_rules()
            elif event.key == pygame.K_c:
                generate_colors()


    for blob in blobs:
        blob.interactions(blobs)
    for blob in blobs:
        blob.update()

    screen.fill((40, 40, 40))
    for blob in blobs:
        blob.draw(screen)
    
    draw_rules(screen, rule_matrix, (700, 30), size=20)

    # print fps in top left corner
    fps_text = font.render(str(int(clock.get_fps())), False, (255, 255, 255))
    screen.blit(fps_text, (0, 0))
        

    pygame.display.flip()
    clock.tick(60)

pygame.quit()
sys.exit()
