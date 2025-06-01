#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cstdlib>  // system()
#include <sstream>

int main() {
    std::vector<int> lens_positions = { 0, 4, 8, 12, 16, 20, 24, 28, 32 };
    double best_score = 0.0;
    int best_pos = 0;

    for (int pos : lens_positions) {
        std::stringstream filename, command;

        // Görüntü dosyasının adını belirle (örnek: focus_12.jpg)
        filename << "focus_" << pos << ".jpg";

        // Kameradan görüntü çek (manuel odak modunda)
        command << "libcamera-still --autofocus-mode manual --lens-position "
            << pos << " -o " << filename.str() << " --timeout 1000 -n";
        std::cout << "[+] Komut çalışıyor: " << command.str() << std::endl;

        int ret = system(command.str().c_str());
        if (ret != 0) {
            std::cerr << "Görüntü alınamadı, lens pos: " << pos << std::endl;
            continue;
        }

        // Görüntüyü gri tonlamalı olarak oku
        cv::Mat img = cv::imread(filename.str(), cv::IMREAD_GRAYSCALE);
        if (img.empty()) {
            std::cerr << "Dosya okunamadı: " << filename.str() << std::endl;
            continue;
        }

        // Laplacian filtre uygula → yüksek frekans (kenar bilgisi) çıkarılır
        // Kenarlar = ani parlaklık değişimi = keskinlik
        cv::Mat lap;
        cv::Laplacian(img, lap, CV_64F);

        // Standart sapma hesapla (kenar bilgisi yayılımı)
        cv::Scalar mean, stddev;
        cv::meanStdDev(lap, mean, stddev);
        double focus_score = stddev[0] * stddev[0];

        std::cout << "Lens Position " << pos
            << " → Focus Score: " << focus_score << std::endl;

        // En yüksek skoru takip et
        if (focus_score > best_score) {
            best_score = focus_score;
            best_pos = pos;
        }
    }

    std::cout << "\n[✓] En yüksek netlik skoru: " << best_score
        << " @ lens-position: " << best_pos << std::endl;

    return 0;
}
