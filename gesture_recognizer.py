import cv2
import mediapipe as mp
import keras
import numpy as np

np.set_printoptions(precision=3, suppress=True)

MINIMUM_X = 200
MAXIMUM_X = 440
MINIMUM_Y = 100
MAXIMUM_Y = 380


class GestureRecognizer:
    "Kézjelek felismeréséhez használt objektum-osztály"
    last_gesture = ''

    def __init__(self, model_path):
        self.g_recognizer = keras.models.load_model(model_path)

    def recognize(self, image):
        hand_points = []
        mp_drawing = mp.solutions.drawing_utils
        mp_drawing_styles = mp.solutions.drawing_styles
        mp_hands = mp.solutions.hands
        with mp_hands.Hands(static_image_mode=True, max_num_hands=1, min_detection_confidence=0.5) as hands:
            results = hands.process(cv2.cvtColor(image, cv2.COLOR_BGR2RGB))
            if not results.multi_hand_landmarks:
                return "None!"
            if results.multi_hand_landmarks:
                for hand_landmarks in results.multi_hand_landmarks:
                    for lm in hand_landmarks.landmark:
                        h, w, c = image.shape
                        cx, cy = int(lm.x * w), int(lm.y * h)
                        hand_points.append([cx, cy])
        if not self.check_points(hand_points):
            return "None!"
        return self.normalize(hand_points)

    def normalize(self, hand_points):
        # Referencia pont: Csukló
        ref_x = hand_points[0][0]
        ref_y = hand_points[0][1]
        max_x = 0
        max_y = 0
        # Pontok relativizálva a csukló ponthoz, majd normalizálva -1,1 intervallumban
        for m in range(0, 21):
            hand_points[m][0] = hand_points[m][0] - ref_x
            hand_points[m][1] = hand_points[m][1] - ref_y
            if abs(hand_points[m][0]) > abs(max_x):
                max_x = abs(hand_points[m][0])
            if abs(hand_points[m][1]) > abs(max_y):
                max_y = abs(hand_points[m][1])
        for n in range(0, 21):
            hand_points[n][0] = hand_points[n][0] / max_x
            hand_points[n][1] = hand_points[n][1] / max_y
        return self.use_ml_model(hand_points)

    def use_ml_model(self, normalized_points):
        data_vector = np.array(normalized_points)
        data_vector = data_vector.flatten()
        data_vector = np.array(data_vector, ndmin=2)
        result = self.g_recognizer.predict(data_vector)
        if result.max() > 0.90:
            return result.argmax()
        else:
            return "None!"

    def check_points(self, points):
        for m in range(0, 21):
            if (points[m][0] < MINIMUM_X or points[m][0] > MAXIMUM_X) or (points[m][1] < MINIMUM_Y or points[m][1] > MAXIMUM_Y):
                return False
        return True
