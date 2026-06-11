/**
 *
 *  @file PdfController.cpp
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

#include <pico/presentation/controllers/PdfController.hpp>
#include <pico/support/HttpQuery.hpp>

#include <string>

namespace pico::presentation::controllers
{
  namespace
  {
    void send_pdf(
        vix::Response &res,
        const application::services::GeneratedPdf &pdf)
    {
      if (!pdf.ok)
      {
        res.status(vix::http::INTERNAL_ERROR)
            .json(pdf.error);
        return;
      }

      res.header("Content-Type", pdf.content_type);
      res.header(
          "Content-Disposition",
          "attachment; filename=\"" + pdf.filename + "\"");

      res.send(pdf.bytes);
    }
  }

  void PdfController::register_routes(
      vix::App &app,
      application::services::PdfService &pdfs)
  {
    app.get("/api/pdf/basic",
            [&pdfs](vix::Request &, vix::Response &res)
            {
              const auto pdf = pdfs.basic_pdf();

              send_pdf(res, pdf);
            });

    app.get("/api/pdf/events",
            [&pdfs](vix::Request &req, vix::Response &res)
            {
              const auto pagination = support::read_pagination(
                  req,
                  25,
                  100);

              const auto pdf = pdfs.events_pdf(pagination.limit);

              send_pdf(res, pdf);
            });
  }
}
