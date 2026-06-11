/**
 *
 *  @file PdfService.cpp
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

#include <pico/application/services/PdfService.hpp>

#include <algorithm>
#include <string>

namespace pico::application::services
{
  namespace
  {
    [[nodiscard]] std::string event_summary(
        const domain::models::Event &event)
    {
      return std::to_string(event.id) + " | " +
             event.source + " | " +
             event.type + " | " +
             event.created_at;
    }
  }

  PdfService::PdfService(EventService &events)
      : events_(&events)
  {
  }

  GeneratedPdf PdfService::basic_pdf()
  {
    using namespace vix::json;

    auto doc = rix.pdf.document();

    doc.set_title("Pico PDF Diagnostic")
        .set_author("Pico")
        .set_subject("Production rix/pdf diagnostic")
        .set_keywords("pico,vix,rix,pdf,cpp");

    auto &page = doc.add_page();

    auto y = page.heading(
        page.x_left(),
        page.y_top(),
        "Pico PDF diagnostic",
        1);

    y -= 10.0F;

    page.paragraph(
        page.x_left(),
        y,
        page.content_width(),
        "This PDF was generated in production by Pico using rix/pdf through the unified Rix facade.");

    y -= 60.0F;

    page.paragraph(
        page.x_left(),
        y,
        page.content_width(),
        "Pico validates Vix.cpp runtime features such as HTTP routes, durable KV state, SQLite events, ThreadPool jobs, WebSocket diagnostics, and PDF generation.");

    auto written = rix.pdf.write(doc);

    if (written.failed())
    {
      const auto &error = written.error();

      (void)events_->record(
          "pdf",
          "pdf.basic.failed",
          dumps_compact(o(
              "error", std::string{rix.pdf.error.to_string(error)},
              "message", error.message())));

      return make_error_pdf_result(
          std::string{rix.pdf.error.to_string(error)},
          error.message());
    }

    (void)events_->record(
        "pdf",
        "pdf.basic.generated",
        "{}");

    GeneratedPdf output{};
    output.ok = true;
    output.filename = "pico-basic.pdf";
    output.bytes = written.value();

    return output;
  }

  GeneratedPdf PdfService::events_pdf(std::size_t limit)
  {
    using namespace vix::json;

    const auto safe_limit = std::clamp<std::size_t>(limit, 1, 100);
    const auto latest_events = events_->latest(safe_limit);

    auto doc = rix.pdf.document();

    doc.set_title("Pico Runtime Events")
        .set_author("Pico")
        .set_subject("Runtime events generated from SQLite")
        .set_keywords("pico,vix,rix,pdf,events,sqlite");

    auto &page = doc.add_page();

    auto y = page.heading(
        page.x_left(),
        page.y_top(),
        "Pico runtime events",
        1);

    y -= 10.0F;

    page.paragraph(
        page.x_left(),
        y,
        page.content_width(),
        "This PDF contains recent runtime events recorded by Pico in SQLite.");

    y -= 40.0F;

    rixlib::pdf::Table table{};

    table.set_column_widths({
        60.0F,
        90.0F,
        150.0F,
        180.0F,
    });

    table.add_header({
        "ID",
        "Source",
        "Type",
        "Created",
    });

    for (const auto &event : latest_events)
    {
      table.add_row({
          std::to_string(event.id),
          event.source,
          event.type,
          event.created_at,
      });
    }

    page.table(
        page.x_left(),
        y,
        table);

    auto written = rix.pdf.write(doc);

    if (written.failed())
    {
      const auto &error = written.error();

      (void)events_->record(
          "pdf",
          "pdf.events.failed",
          dumps_compact(o(
              "error", std::string{rix.pdf.error.to_string(error)},
              "message", error.message(),
              "limit", safe_limit)));

      return make_error_pdf_result(
          std::string{rix.pdf.error.to_string(error)},
          error.message());
    }

    (void)events_->record(
        "pdf",
        "pdf.events.generated",
        dumps_compact(o(
            "count", latest_events.size(),
            "limit", safe_limit)));

    GeneratedPdf output{};
    output.ok = true;
    output.filename = "pico-events.pdf";
    output.bytes = written.value();

    return output;
  }

  GeneratedPdf PdfService::make_error_pdf_result(
      const std::string &code,
      const std::string &message) const
  {
    using namespace vix::json;

    GeneratedPdf output{};
    output.ok = false;
    output.error = o(
        "ok", false,
        "error", o("code", code, "message", message));

    return output;
  }
}
