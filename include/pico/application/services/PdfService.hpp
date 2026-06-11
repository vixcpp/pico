/**
 *
 *  @file PdfService.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/pico
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Pico
 *
 */

#ifndef PICO_APPLICATION_SERVICES_PDF_SERVICE_HPP
#define PICO_APPLICATION_SERVICES_PDF_SERVICE_HPP

#include <string>

#include <pico/application/services/EventService.hpp>

#include <rix.hpp>
#include <vix/json.hpp>

namespace pico::application::services
{
  /**
   * @brief Generated PDF payload.
   */
  struct GeneratedPdf
  {
    bool ok{false};
    std::string filename{};
    std::string content_type{"application/pdf"};
    std::string bytes{};
    vix::json::Json error{};
  };

  /**
   * @brief Application service for rix/pdf production diagnostics.
   *
   * PdfService wraps the Rix PDF facade so Pico can generate PDF responses
   * from production routes while keeping controller logic small.
   */
  class PdfService
  {
  public:
    /**
     * @brief Creates a PDF service.
     *
     * @param events Event service used to record PDF diagnostics.
     */
    explicit PdfService(EventService &events);

    PdfService(const PdfService &) = delete;
    PdfService &operator=(const PdfService &) = delete;

    PdfService(PdfService &&) noexcept = default;
    PdfService &operator=(PdfService &&) noexcept = default;

    ~PdfService() = default;

    /**
     * @brief Generates a basic rix/pdf diagnostic document.
     *
     * @return Generated PDF payload.
     */
    [[nodiscard]] GeneratedPdf basic_pdf();

    /**
     * @brief Generates a PDF containing recent Pico runtime events.
     *
     * @param limit Maximum number of events to include.
     * @return Generated PDF payload.
     */
    [[nodiscard]] GeneratedPdf events_pdf(std::size_t limit = 25);

  private:
    [[nodiscard]] GeneratedPdf make_error_pdf_result(
        const std::string &code,
        const std::string &message) const;

  private:
    EventService *events_;
  };
}

#endif // PICO_APPLICATION_SERVICES_PDF_SERVICE_HPP
