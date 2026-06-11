/**
 *
 *  @file PdfController.hpp
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

#ifndef PICO_PRESENTATION_CONTROLLERS_PDF_CONTROLLER_HPP
#define PICO_PRESENTATION_CONTROLLERS_PDF_CONTROLLER_HPP

#include <pico/application/services/PdfService.hpp>

#include <vix.hpp>

namespace pico::presentation::controllers
{
  /**
   * @brief Registers rix/pdf production diagnostic routes.
   *
   * PdfController exposes small HTTP endpoints that generate PDF documents
   * through PdfService. It is used to validate rix/pdf in production through
   * the Vix HTTP runtime.
   */
  class PdfController
  {
  public:
    /**
     * @brief Registers PDF diagnostic routes on the Vix application.
     *
     * @param app Vix application.
     * @param pdfs PDF application service.
     */
    static void register_routes(
        vix::App &app,
        application::services::PdfService &pdfs);
  };
}

#endif // PICO_PRESENTATION_CONTROLLERS_PDF_CONTROLLER_HPP
